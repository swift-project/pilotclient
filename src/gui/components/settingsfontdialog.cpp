// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsfontdialog.h"

#include "ui_settingsfontdialog.h"

namespace swift::gui::components
{
    CSettingsFontDialog::CSettingsFontDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CSettingsFontDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        ui->comp_FontSettings->setMode(CSettingsFontComponent::GenerateQssOnly);

        connect(ui->comp_FontSettings, &CSettingsFontComponent::accept, this, &CSettingsFontDialog::accept);
        connect(ui->comp_FontSettings, &CSettingsFontComponent::reject, this, &CSettingsFontDialog::reject);
    }

    CSettingsFontDialog::~CSettingsFontDialog() {}

    const QString &CSettingsFontDialog::getQss() const { return ui->comp_FontSettings->getQss(); }

    void CSettingsFontDialog::setFont(const QFont &font) { ui->comp_FontSettings->setFont(font); }

    QFont CSettingsFontDialog::getFont() const { return ui->comp_FontSettings->getFont(); }

    QStringList CSettingsFontDialog::getFamilySizeStyle() const { return ui->comp_FontSettings->getFamilySizeStyle(); }

    void CSettingsFontDialog::setWithColorSelection(bool withColor)
    {
        ui->comp_FontSettings->setWithColorSelection(withColor);
    }
} // namespace swift::gui::components
