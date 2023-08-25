// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "coresettingsdialog.h"
#include "ui_coresettingsdialog.h"
#include "blackgui/guiapplication.h"
#include <QPushButton>

namespace BlackGui::Components
{
    CCoreSettingsDialog::CCoreSettingsDialog(QWidget *parent) : QDialog(parent),
                                                                ui(new Ui::CCoreSettingsDialog)
    {
        Q_ASSERT(sGui);
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        QPushButton *overview = ui->bb_CoreSettingsDialog->button(QDialogButtonBox::Reset);
        overview->setText("Overview");
        connect(overview, &QPushButton::released, this, &CCoreSettingsDialog::showOverview);
    }

    CCoreSettingsDialog::~CCoreSettingsDialog()
    {}

    void CCoreSettingsDialog::showOverview()
    {
        ui->comp_SettingsComponent->setSettingsOverviewTab();
    }
} // ns
