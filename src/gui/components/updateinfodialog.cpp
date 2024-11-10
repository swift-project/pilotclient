// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "updateinfodialog.h"
#include "ui_updateinfodialog.h"
#include "gui/guiapplication.h"
#include <QPushButton>
#include <QDesktopServices>

using namespace swift::misc::db;

namespace swift::gui::components
{
    CUpdateInfoDialog::CUpdateInfoDialog(QWidget *parent) : QDialog(parent),
                                                            ui(new Ui::CUpdateInfoDialog)
    {
        ui->setupUi(this);
        ui->bb_UpdateInfolDialog->button(QDialogButtonBox::Ok)->setText(" Download and install ");
        ui->cb_DontShowAgain->setChecked(!m_setting.get());
        this->selectionChanged();
        connect(ui->comp_UpdateInfo, &CUpdateInfoComponent::selectionChanged, this, &CUpdateInfoDialog::selectionChanged);
        connect(ui->cb_DontShowAgain, &QCheckBox::toggled, this, &CUpdateInfoDialog::onDontShowAgain);
    }

    CUpdateInfoDialog::~CUpdateInfoDialog()
    {}

    bool CUpdateInfoDialog::isNewVersionAvailable() const
    {
        const bool newVersion = ui->comp_UpdateInfo->isNewPilotClientVersionAvailable();
        return newVersion;
    }

    int CUpdateInfoDialog::exec()
    {
        const int r = QDialog::exec();
        if (r != QDialog::Accepted) { return r; }
        if (!ui->comp_UpdateInfo->isNewPilotClientVersionAvailable()) { return QDialog::Rejected; }
        const CDistribution distribution = ui->comp_UpdateInfo->getCurrentDistribution();
        if (!distribution.hasDownloadUrl()) { return QDialog::Rejected; }

        ui->comp_UpdateInfo->triggerDownload();
        return r;
    }

    bool CUpdateInfoDialog::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }

    void CUpdateInfoDialog::onDontShowAgain(bool dontShowAgain)
    {
        m_setting.setAndSave(!dontShowAgain);
    }

    void CUpdateInfoDialog::selectionChanged()
    {
        const bool nv = ui->comp_UpdateInfo->isNewPilotClientVersionAvailable();
        ui->bb_UpdateInfolDialog->button(QDialogButtonBox::Ok)->setVisible(nv);
    }
} // ns
