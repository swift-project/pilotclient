/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QTimer>

using namespace BlackMisc::Network;

namespace BlackGui::Components
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
