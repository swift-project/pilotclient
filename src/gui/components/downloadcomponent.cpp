// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "downloadcomponent.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointer>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

#include "ui_downloadcomponent.h"

#include "config/buildconfig.h"
#include "gui/guiapplication.h"
#include "gui/overlaymessagesframe.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"
#include "misc/logmessage.h"
#include "misc/simulation/xplane/xplaneutil.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CDownloadComponent::CDownloadComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), CLoadIndicatorEnabled(this), ui(new Ui::CDownloadComponent)
    {
        ui->setupUi(this);
        this->setOverlaySizeFactors(0.8, 0.9);
        this->setForceSmall(true);

        ui->le_DownloadDir->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        ui->prb_Current->setMinimum(0);
        ui->prb_Current->setMaximum(1); // min/max 0,0 means busy indicator
        ui->prb_Current->setValue(0);
        ui->prb_Total->setMinimum(0);
        ui->prb_Total->setMaximum(1);
        ui->prb_Total->setValue(0);

        connect(ui->tb_DialogDownloadDir, &QToolButton::pressed, this, &CDownloadComponent::selectDownloadDirectory);
        connect(ui->tb_ResetDownloadDir, &QToolButton::pressed, this, &CDownloadComponent::resetDownloadDir);
        connect(ui->tb_CancelDownload, &QToolButton::pressed, this, &CDownloadComponent::cancelOngoingDownloads);
        connect(ui->pb_Download, &QPushButton::pressed, [=] { this->triggerDownloadingOfFiles(); });
        connect(ui->pb_OpenDownloadDir, &QPushButton::pressed, this, &CDownloadComponent::openDownloadDir);
        connect(ui->pb_Launch, &QPushButton::pressed, this, &CDownloadComponent::startDownloadedExecutable);
    }

    CDownloadComponent::~CDownloadComponent() {}

    bool CDownloadComponent::setDownloadFile(const CRemoteFile &remoteFile)
    {
        return this->setDownloadFiles(CRemoteFileList { remoteFile });
    }

    bool CDownloadComponent::setDownloadFiles(const CRemoteFileList &remoteFiles)
    {
        if (!m_waitingForDownload.isEmpty()) { return false; }
        m_remoteFiles = remoteFiles;
        this->clear();
        return true;
    }

    bool CDownloadComponent::setDownloadDirectory(const QString &path)
    {
        const QDir d(path);
        if (!d.exists()) return false;
        ui->le_DownloadDir->setText(d.absolutePath());
        return true;
    }

    void CDownloadComponent::selectDownloadDirectory()
    {
        QString downloadDir = ui->le_DownloadDir->text().trimmed();
        downloadDir = QFileDialog::getExistingDirectory(parentWidget(), tr("Choose your download directory"),
                                                        downloadDir, m_fileDialogOptions);

        if (downloadDir.isEmpty()) { return; } // canceled
        if (!QDir(downloadDir).exists())
        {
            const CStatusMessage msg =
                CStatusMessage(this, CLogCategories::validation()).warning(u"'%1' is not a valid download directory")
                << downloadDir;
            this->showOverlayMessage(msg, CDownloadComponent::OverlayMsgTimeout);
            return;
        }
        ui->le_DownloadDir->setText(downloadDir);
    }

    bool CDownloadComponent::triggerDownloadingOfFiles(int delayMs)
    {
        ui->pb_Download->setEnabled(false);
        ui->pb_Launch->setEnabled(false);
        if (m_remoteFiles.isEmpty()) { return false; }
        if (!m_waitingForDownload.isEmpty()) { return false; }
        if (delayMs > 0)
        {
            const QPointer<CDownloadComponent> myself(this);
            QTimer::singleShot(delayMs, this, [=] {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                this->triggerDownloadingOfFiles();
            });
            return true;
        }
        m_waitingForDownload = m_remoteFiles;
        this->showFileInfo();
        return this->triggerDownloadingOfNextFile();
    }

    bool CDownloadComponent::isDownloading() const { return m_reply || m_fileInProgress.hasName(); }

    bool CDownloadComponent::haveAllDownloadsCompleted() const
    {
        if (this->isDownloading()) { return false; }
        if (!m_waitingForDownload.isEmpty()) { return false; }
        return true;
    }

    CDownloadComponent::Mode CDownloadComponent::getMode() const
    {
        Mode mode = ui->cb_Shutdown->isChecked() ? ShutdownSwift : JustDownload;
        if (ui->cb_StartAfterDownload) { mode |= StartAfterDownload; }
        return mode;
    }

    void CDownloadComponent::setMode(Mode mode)
    {
        ui->cb_Shutdown->setChecked(mode.testFlag(ShutdownSwift));
        ui->cb_StartAfterDownload->setChecked(mode.testFlag(StartAfterDownload));
    }

    void CDownloadComponent::clear()
    {
        if (m_reply)
        {
            m_reply->abort();
            m_reply = nullptr;
        }

        m_waitingForDownload.clear();
        m_fileInProgress = CRemoteFile();

        ui->prb_Current->setValue(0);
        ui->prb_Total->setValue(0);

        ui->le_Completed->clear();
        ui->le_CompletedNumber->clear();
        ui->le_CompletedUrl->clear();
        ui->le_Started->clear();
        ui->le_StartedNumber->clear();
        ui->le_StartedUrl->clear();
        this->showFileInfo();
        ui->pb_Download->setEnabled(true);
    }

    bool CDownloadComponent::triggerDownloadingOfNextFile()
    {
        if (m_waitingForDownload.isEmpty()) { return false; }
        const CRemoteFile rf = m_waitingForDownload.front();
        m_waitingForDownload.pop_front();
        return this->triggerDownloadingOfFile(rf);
    }

    bool CDownloadComponent::triggerDownloadingOfFile(const CRemoteFile &remoteFile)
    {
        if (!sGui || !sGui->hasWebDataServices() || sGui->isShuttingDown()) { return false; }
        if (!this->existsDownloadDir())
        {
            const CStatusMessage msg =
                CStatusMessage(this, CLogCategories::validation()).error(u"Invalid download directory");
            this->showOverlayMessage(msg, CDownloadComponent::OverlayMsgTimeout);
            return false;
        }

        const CUrl download = remoteFile.getSmartUrl();
        if (download.isEmpty())
        {
            const CStatusMessage msg =
                CStatusMessage(this, CLogCategories::validation()).error(u"No download URL for file name '%1'")
                << remoteFile.getBaseNameAndSize();
            this->showOverlayMessage(msg, CDownloadComponent::OverlayMsgTimeout);
            return false;
        }

        this->showStartedFileMessage(remoteFile);
        m_fileInProgress = remoteFile;
        const QString saveAsFile = CFileUtils::appendFilePaths(ui->le_DownloadDir->text(), remoteFile.getBaseName());
        const QFileInfo fiSaveAs(saveAsFile);
        if (fiSaveAs.exists())
        {
            const QString msg = QStringLiteral("File '%1' already exists locally.\n\nDo you want to reload the file?")
                                    .arg(fiSaveAs.absoluteFilePath());
            QMessageBox::StandardButton reply =
                QMessageBox::question(this, "File exists", msg, QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes)
            {
                const QPointer<CDownloadComponent> myself(this);
                QTimer::singleShot(10, this, [=] {
                    if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                    this->downloadedFile(CStatusMessage(this).info(u"File was already downloaded"));
                });
                return true;
            }
        }

        QNetworkReply *reply =
            sGui->downloadFromNetwork(download, saveAsFile, { this, &CDownloadComponent::downloadedFile });
        bool success = false;
        if (reply)
        {
            // this->showLoading(10 * 1000);
            CLogMessage(this).info(u"Triggered downloading of file from '%1'") << download.getHost();
            connect(reply, &QNetworkReply::downloadProgress, this, &CDownloadComponent::downloadProgress,
                    Qt::QueuedConnection);
            m_reply = reply;
            success = true;
        }
        else
        {
            const CStatusMessage msg =
                CStatusMessage(this, CLogCategories::validation()).error(u"Starting download for '%1' failed")
                << download.getFullUrl();
            this->showOverlayMessage(msg, CDownloadComponent::OverlayMsgTimeout);
        }
        return success;
    }

    void CDownloadComponent::downloadedFile(const CStatusMessage &status)
    {
        // reset in progress
        const CRemoteFile justDownloaded(m_fileInProgress);
        m_fileInProgress = CRemoteFile();
        m_reply = nullptr;
        this->showCompletedFileMessage(justDownloaded);
        this->hideLoading();

        if (sGui && sGui->isShuttingDown()) { return; }
        if (status.isWarningOrAbove())
        {
            this->showOverlayMessage(status, CDownloadComponent::OverlayMsgTimeout);
            this->clear();
            return;
        }

        const bool t = this->triggerDownloadingOfNextFile();
        if (!t) { this->lastFileDownloaded(); }
    }

    void CDownloadComponent::lastFileDownloaded()
    {
        const QPointer<CDownloadComponent> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            myself->ui->pb_Download->setEnabled(true);
            myself->ui->pb_Launch->setEnabled(true);
            emit allDownloadsCompleted();
        });
        this->startDownloadedExecutable();
    }

    void CDownloadComponent::startDownloadedExecutable()
    {
        if (!ui->cb_StartAfterDownload->isChecked()) { return; }
        if (!this->haveAllDownloadsCompleted()) { return; }
        const CRemoteFileList executables = m_remoteFiles.findExecutableFiles();
        if (executables.isEmpty()) { return; }

        // try to start
        const QDir dir(ui->le_DownloadDir->text());
        if (!dir.exists()) { return; }

        QString msg;
        if (CBuildConfig::isRunningOnMacOSPlatform())
        {
            msg = "To install close swift, "
                  "mount the disk image '%1' and run the installer inside "
                  "to proceed with the update.";
        }
        else { msg = ui->cb_Shutdown->isChecked() ? QString("Start '%1' and close swift?") : QString("Start '%1'?"); }

        for (const CRemoteFile &rf : executables)
        {
            const QString executable = CFileUtils::appendFilePaths(dir.absolutePath(), rf.getBaseName());
            QFile executableFile(executable);
            if (!executableFile.exists()) { continue; }

            QMessageBox::StandardButton reply =
                QMessageBox::question(this, "Start?", msg.arg(rf.getName()), QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }

            const CPlatform p = CArtifact::artifactNameToPlatform(rf.getName());
            if (!CPlatform::canRunOnCurrentPlatform(p))
            {
                // cannot run on this OS, just show the directory where the download resides
                // do not close
                ui->pb_OpenDownloadDir->click();
                return;
            }

            if (CBuildConfig::isRunningOnLinuxPlatform() && !executableFile.permissions().testFlag(QFile::ExeOwner))
            {
                executableFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                                              QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);
            }

            const bool shutdown = ui->cb_Shutdown->isChecked();
            const bool started = QProcess::startDetached(executable, {}, dir.absolutePath());
            if (started && shutdown && sGui)
            {
                QTimer::singleShot(250, sGui, [] {
                    if (!sGui) { return; }
                    CGuiApplication::exit();
                });
                break;
            }
        } // files
    }

    bool CDownloadComponent::existsDownloadDir() const
    {
        if (ui->le_DownloadDir->text().isEmpty()) { return false; }
        const QDir dir(ui->le_DownloadDir->text());
        return dir.exists() && dir.isReadable();
    }

    void CDownloadComponent::openDownloadDir()
    {
        if (!this->existsDownloadDir()) { return; }
        QDesktopServices::openUrl(QUrl::fromLocalFile(ui->le_DownloadDir->text()));
    }

    void CDownloadComponent::resetDownloadDir()
    {
        ui->le_DownloadDir->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    }

    void CDownloadComponent::showStartedFileMessage(const CRemoteFile &rf)
    {
        const int current = m_remoteFiles.size() - m_waitingForDownload.size();
        ui->le_Started->setText(rf.getBaseName());
        ui->le_StartedNumber->setText(QStringLiteral("%1/%2").arg(current).arg(m_remoteFiles.size()));
        ui->le_StartedUrl->setText(rf.getUrl().getFullUrl());
        ui->prb_Total->setMaximum(m_remoteFiles.size());
        ui->prb_Total->setValue(current - 1);
    }

    void CDownloadComponent::showCompletedFileMessage(const CRemoteFile &rf)
    {
        const int current = m_remoteFiles.size() - m_waitingForDownload.size();
        ui->le_Completed->setText(rf.getBaseName());
        ui->le_CompletedNumber->setText(QStringLiteral("%1/%2").arg(current).arg(m_remoteFiles.size()));
        ui->le_CompletedUrl->setText(rf.getUrl().getFullUrl());
        ui->prb_Total->setMaximum(m_remoteFiles.size());
        ui->prb_Total->setValue(current);
    }

    void CDownloadComponent::cancelOngoingDownloads() { this->clear(); }

    void CDownloadComponent::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
    {
        ui->prb_Current->setMaximum(static_cast<int>(bytesTotal));
        ui->prb_Current->setValue(static_cast<int>(bytesReceived));
    }

    void CDownloadComponent::showFileInfo()
    {
        ui->le_Info->setText(QStringLiteral("Files: %1 size: %2")
                                 .arg(m_remoteFiles.size())
                                 .arg(m_remoteFiles.getTotalFileSizeHumanReadable()));
    }
} // namespace swift::gui::components
