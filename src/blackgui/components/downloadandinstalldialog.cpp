/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "downloadandinstalldialog.h"
#include "ui_downloadandinstalldialog.h"
#include <QPushButton>
#include <QDesktopServices>

using namespace BlackMisc::Db;

namespace BlackGui
{
    class COverlayMessagesFrame;

    namespace Components
    {
        CDownloadAndInstallDialog::CDownloadAndInstallDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CDownloadAndInstallDialog)
        {
            ui->setupUi(this);
            ui->bb_DownloadInstallDialog->button(QDialogButtonBox::Ok)->setText(" Download and install ");
            this->selectionChanged();
            connect(ui->comp_DistributionInfo, &CDistributionInfoComponent::selectionChanged, this, &CDownloadAndInstallDialog::selectionChanged);
        }

        CDownloadAndInstallDialog::~CDownloadAndInstallDialog()
        { }

        bool CDownloadAndInstallDialog::isNewVersionAvailable() const
        {
            const bool newVersion = ui->comp_DistributionInfo->isNewVersionAvailable();
            return newVersion;
        }

        int CDownloadAndInstallDialog::exec()
        {
            const int r = QDialog::exec();
            if (r != QDialog::Accepted) return r;
            if (!ui->comp_DistributionInfo->isNewVersionAvailable()) { return QDialog::Rejected; }
            const CDistribution distribution = ui->comp_DistributionInfo->getCurrentDistribution();
            if (!distribution.hasDownloadUrls()) { return QDialog::Rejected; }

            // in future, start download and close application
            // for now, just open URL
            QDesktopServices::openUrl(distribution.getDownloadUrls().getRandomUrl());
            return QDialog::Rejected;
        }

        void CDownloadAndInstallDialog::selectionChanged()
        {
            const bool nv = ui->comp_DistributionInfo->isNewVersionAvailable();
            ui->bb_DownloadInstallDialog->button(QDialogButtonBox::Ok)->setEnabled(nv);
        }
    } // ns
} // ns
