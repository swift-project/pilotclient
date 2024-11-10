// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QTimer>

using namespace swift::misc::network;

namespace swift::gui::components
{
    CDownloadDialog::CDownloadDialog(QWidget *parent) : QDialog(parent),
                                                        ui(new Ui::CDownloadDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CDownloadDialog::~CDownloadDialog()
    {}

    void CDownloadDialog::setDownloadFile(const CRemoteFile &remoteFile)
    {
        setWindowTitle("Downloading " + remoteFile.getName());
        ui->comp_Download->setDownloadFile(remoteFile);
    }

    void CDownloadDialog::setDownloadFiles(const CRemoteFileList &remoteFiles)
    {
        ui->comp_Download->setDownloadFiles(remoteFiles);
    }

    void CDownloadDialog::triggerDownloadingOfFiles(int delayMs)
    {
        ui->comp_Download->triggerDownloadingOfFiles(delayMs);
    }

    void CDownloadDialog::setMode(CDownloadComponent::Mode mode)
    {
        ui->comp_Download->setMode(mode);
    }

    void CDownloadDialog::showAndStartDownloading()
    {
        const QPointer<CDownloadDialog> guard(this);
        QTimer::singleShot(0, this, [=] {
            if (guard.isNull()) { return; }
            ui->comp_Download->triggerDownloadingOfFiles(2500);
        });
        this->show();
    }

    void CDownloadDialog::accept()
    {
        if (!ui->comp_Download->haveAllDownloadsCompleted())
        {
            const QString msg = QStringLiteral("Download ongoing. Do you want to abort it?");
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Abort?", msg, QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                ui->comp_Download->cancelOngoingDownloads();
                this->done(CDownloadDialog::Rejected);
            }
        }
        else
        {
            this->done(CDownloadDialog::Accepted);
        }
    }

} // ns
