// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "installxswiftbuscomponent.h"
#include "ui_installxswiftbuscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/compressutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "config/buildconfig.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTimer>
#include <QPointer>
#include <QDesktopServices>

using namespace swift::config;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackGui::Components
{
    CInstallXSwiftBusComponent::CInstallXSwiftBusComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                              CLoadIndicatorEnabled(this),
                                                                              ui(new Ui::CInstallXSwiftBusComponent)
    {
        ui->setupUi(this);

        ui->le_XSwiftBusPluginDir->setText(this->getXPlanePluginDirectory());
        ui->le_DownloadDir->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        ui->cb_DownloadFile->setEnabled(false);

        connect(ui->tb_DialogInstallDir, &QPushButton::pressed, this, &CInstallXSwiftBusComponent::selectPluginDirectory);
        connect(ui->tb_DialogDownloadDir, &QPushButton::pressed, this, &CInstallXSwiftBusComponent::selectDownloadDirectory);
        connect(ui->pb_Download, &QPushButton::pressed, this, &CInstallXSwiftBusComponent::triggerDownloadingOfXSwiftBusFile);
        connect(ui->pb_OpenDownloadDir, &QPushButton::pressed, this, &CInstallXSwiftBusComponent::openDownloadDir);
        connect(ui->pb_OpenInstallDir, &QPushButton::pressed, this, &CInstallXSwiftBusComponent::openInstallDir);

        // init upate info
        this->updatesChanged();
    }

    CInstallXSwiftBusComponent::~CInstallXSwiftBusComponent()
    {}

    void CInstallXSwiftBusComponent::setDefaultDownloadName(const QString &defaultDownload)
    {
        m_defaultDownloadName = defaultDownload;
        this->updatesChanged();
    }

    void CInstallXSwiftBusComponent::selectPluginDirectory()
    {
        QString xPlanePluginDir = CFileUtils::fixWindowsUncPath(ui->le_XSwiftBusPluginDir->text().trimmed());
        xPlanePluginDir = QFileDialog::getExistingDirectory(parentWidget(),
                                                            tr("Choose your X-Plane plugin directory"), xPlanePluginDir, m_fileDialogOptions);

        if (xPlanePluginDir.isEmpty()) { return; } // canceled
        if (!QDir(xPlanePluginDir).exists())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).warning(u"'%1' is not a valid X-Plane plugin directory") << xPlanePluginDir;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }
        ui->le_XSwiftBusPluginDir->setText(xPlanePluginDir);
    }

    void CInstallXSwiftBusComponent::selectDownloadDirectory()
    {
        QString downloadDir = CFileUtils::fixWindowsUncPath(ui->le_DownloadDir->text().trimmed());
        downloadDir = QFileDialog::getExistingDirectory(parentWidget(),
                                                        tr("Choose your X-Plane plugin directory"), downloadDir, m_fileDialogOptions);

        if (downloadDir.isEmpty()) { return; } // canceled
        if (!QDir(downloadDir).exists())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).warning(u"'%1' is not a valid download directory") << downloadDir;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }
        ui->le_DownloadDir->setText(downloadDir);
    }

    void CInstallXSwiftBusComponent::installXSwiftBus()
    {
        const CRemoteFile rf = this->getRemoteFileSelected();
        const QString downloadFileName = CFileUtils::appendFilePathsAndFixUnc(this->downloadDir(), rf.getBaseName());
        QPointer<CInstallXSwiftBusComponent> myself(this);
        QFile downloadFile(downloadFileName);

        if (!downloadFile.exists())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Cannot read downloaded file '%1'") << downloadFileName;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        const QString xSwiftBusDirectory = this->xSwiftBusDir();
        if (xSwiftBusDirectory.isEmpty())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"No directory to install to'");
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        const QDir installDir(xSwiftBusDirectory);
        if (!installDir.exists())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Directory '%1' does not exist") << xSwiftBusDirectory;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        const QString destFileName = CFileUtils::appendFilePathsAndFixUnc(xSwiftBusDirectory, rf.getBaseName());
        {
            QFile destFile(destFileName);
            if (destFile.exists())
            {
                const bool removed = destFile.remove();
                if (!removed)
                {
                    const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Cannot remove '%1'") << destFileName;
                    this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
                    return;
                }
            }
        }

        const bool copied = QFile::copy(downloadFileName, destFileName);
        if (!copied)
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Cannot copy '%1' to '%2'") << downloadFileName << destFileName;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        // we need to unzip the destination file
        const QFileInfo destFile(destFileName);
        if (!destFile.exists())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"xswiftbus file '%1' does not exist") << destFileName;
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        // if possible we will unzip
        QStringList stdOutAndError;
        if (CCompressUtils::zip7Uncompress(destFile.absoluteFilePath(), xSwiftBusDirectory, &stdOutAndError))
        {
            // capture values by copy!
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).info(u"Uncompressed xswiftbus in '%1'") << xSwiftBusDirectory;
            this->showOverlayMessagesWithConfirmation(msg, false, "Delete downloaded file?", [=] {
                if (!myself) { return; }
                QFile downloadFile(downloadFileName);
                if (!downloadFile.exists()) { return; } // removed in meantime
                const bool removed = downloadFile.remove();
                Q_UNUSED(removed);
            });
            return;
        }
        else
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).warning(u"Unzip failed: stdout '%1' stderr '%2'") << safeAt(stdOutAndError, 0) << safeAt(stdOutAndError, 1);
            this->showOverlayMessage(msg);
        }

        //! fixme Ref T253, once we have a zip library we will directly unzip
        const QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                        "Install XSwiftXBus",
                                                                        "You need to manually unzip xswiftbus into the plugins directory.\nIt needs to look like 'plugin/xswiftbus'.\n\nOpen the archive?",
                                                                        QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(destFile.absoluteFilePath()));
        }
    }

    void CInstallXSwiftBusComponent::triggerDownloadingOfXSwiftBusFile()
    {
        if (!sGui || !sGui->hasWebDataServices() || sGui->isShuttingDown()) { return; }
        const CRemoteFile rf = this->getRemoteFileSelected();
        if (!rf.getBaseName().contains(CBuildConfig::getVersionString()))
        {
            const QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                            "Download xswiftbus",
                                                                            QStringLiteral(
                                                                                u"The xswiftbus versions seems to be for a different version\n"
                                                                                u"Your version is '%1'. Use this version.\n\n"
                                                                                u"If not available, you can try the version next to your version number.\n\n"
                                                                                u"Continue with this version?")
                                                                                .arg(CBuildConfig::getVersionString()),
                                                                            QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }
        }

        if (!this->existsDownloadDir())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Invalid download directory");
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        const CUrl download = rf.getSmartUrl();
        if (download.isEmpty())
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"No download URL for file name '%1'") << rf.getBaseNameAndSize();
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
            return;
        }

        const QString saveAsFile = CFileUtils::appendFilePathsAndFixUnc(ui->le_DownloadDir->text(), rf.getBaseName());
        const QFile saveFile(saveAsFile);
        if (saveFile.exists())
        {
            const QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                            "The file already exists",
                                                                            "Do you want to use the existing '" + saveAsFile + "'?",
                                                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel) { return; }
            if (reply == QMessageBox::Yes)
            {
                const CStatusMessage msg = CStatusMessage(this).info(u"Using existing file '%1'") << saveAsFile;
                const QPointer<CInstallXSwiftBusComponent> guard(this);
                QTimer::singleShot(100, this, [=] {
                    if (guard.isNull()) { return; }
                    this->downloadedXSwiftBusFile(msg);
                });
                return;
            }
        }

        const QNetworkReply *r = sGui->downloadFromNetwork(download, saveAsFile, { this, &CInstallXSwiftBusComponent::downloadedXSwiftBusFile });
        if (r)
        {
            CLogMessage(this).info(u"Triggered downloading of xswiftbus file from '%1'") << download.getHost();
            this->showLoading(120 * 1000); // timeout in any case
        }
        else
        {
            const CStatusMessage msg = CStatusMessage(this, CLogCategories::validation()).error(u"Starting download for '%1' failed") << download.getFullUrl();
            this->showOverlayMessage(msg, CInstallXSwiftBusComponent::OverlayMsgTimeoutMs);
        }
    }

    void CInstallXSwiftBusComponent::downloadedXSwiftBusFile(const CStatusMessage &status)
    {
        this->hideLoading();
        if (sGui && sGui->isShuttingDown()) { return; }
        if (status.isWarningOrAbove())
        {
            this->showOverlayMessage(status);
            return;
        }
        if (!this->existsXSwiftBusPluginDir())
        {
            const CStatusMessage msg = CStatusMessage(this).warning(u"No valid install directory, cannot continue.");
            this->showOverlayMessage(msg);
            return;
        }

        static const QString confirm("Install in '%1'?");
        this->showOverlayMessagesWithConfirmation(status, false, confirm.arg(ui->le_XSwiftBusPluginDir->text()), [=] {
            QTimer::singleShot(0, this, &CInstallXSwiftBusComponent::installXSwiftBus);
        });
    }

    CRemoteFile CInstallXSwiftBusComponent::getRemoteFileSelected() const
    {
        const QString baseNameAndSize = ui->cb_DownloadFile->currentText();
        const CUpdateInfo update = m_updates.get();
        const CRemoteFileList remoteFiles = update.getArtifactsXSwiftBus().asRemoteFiles();
        return remoteFiles.findFirstByMatchingBaseNameOrDefault(baseNameAndSize);
    }

    QString CInstallXSwiftBusComponent::downloadDir() const
    {
        return CFileUtils::fixWindowsUncPath(ui->le_DownloadDir->text().trimmed());
    }

    QString CInstallXSwiftBusComponent::xSwiftBusDir() const
    {
        return CFileUtils::fixWindowsUncPath(ui->le_XSwiftBusPluginDir->text().trimmed());
    }

    bool CInstallXSwiftBusComponent::existsDownloadDir() const
    {
        const QDir dir(this->downloadDir());
        return dir.exists() && dir.isReadable();
    }

    bool CInstallXSwiftBusComponent::existsXSwiftBusPluginDir() const
    {
        const QDir dir(this->xSwiftBusDir());
        return dir.exists() && dir.isReadable();
    }

    QString CInstallXSwiftBusComponent::getXPlanePluginDirectory() const
    {
        const CXPlaneSimulatorSettings settings = m_simulatorSettings.getSettings(CSimulatorInfo::XPLANE);
        return settings.getPluginDirOrDefault();
    }

    void CInstallXSwiftBusComponent::updatesChanged()
    {
        const CUpdateInfo updateInfo = m_updates.get();
        if (updateInfo.getArtifactsXSwiftBus().isEmpty()) { return; }
        const CArtifactList artifacts = updateInfo.getArtifactsXSwiftBusLatestVersionFirst().findWithUnrestrictedDistributions();
        if (artifacts.isEmpty()) { return; }

        const CRemoteFileList remoteFiles = artifacts.asRemoteFiles();
        if (!remoteFiles.isEmpty())
        {
            const QStringList xSwiftBusFiles(remoteFiles.getBaseNamesPlusSize(false));
            m_xSwiftBusArtifacts = artifacts;
            ui->cb_DownloadFile->addItems(xSwiftBusFiles);

            // current text
            QString current = xSwiftBusFiles.front(); // default latest first
            if (m_defaultDownloadName.isEmpty())
            {
                const CRemoteFile rf = remoteFiles.findFirstContainingNameOrDefault(CBuildConfig::getVersionString(), Qt::CaseInsensitive);
                if (rf.hasName()) { current = rf.getBaseNameAndSize(); }
            }
            else
            {
                const CRemoteFile rf = remoteFiles.findFirstByMatchingBaseNameOrDefault(m_defaultDownloadName);
                if (rf.hasName()) { current = rf.getBaseNameAndSize(); }
            }

            ui->cb_DownloadFile->setCurrentText(
                current.isEmpty() ?
                    remoteFiles.frontOrDefault().getBaseNameAndSize() :
                    current); // latest version
        }
        ui->cb_DownloadFile->setEnabled(!remoteFiles.isEmpty());
    }

    void CInstallXSwiftBusComponent::openInstallDir()
    {
        if (!this->existsXSwiftBusPluginDir()) { return; }
        const QString file = CFileUtils::fixWindowsUncPath(ui->le_XSwiftBusPluginDir->text());
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }

    void CInstallXSwiftBusComponent::openDownloadDir()
    {
        if (!this->existsDownloadDir()) { return; }
        const QString file = CFileUtils::fixWindowsUncPath(ui->le_DownloadDir->text());
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }

    bool CInstallXSwiftBusWizardPage::validatePage()
    {
        return true;
    }
} // ns
