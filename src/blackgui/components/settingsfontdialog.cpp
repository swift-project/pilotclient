/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsfontdialog.h"
#include "ui_settingsfontdialog.h"

namespace BlackGui::Components
{
    CSettingsFontDialog::CSettingsFontDialog(QWidget *parent) : QDialog(parent),
                                                                ui(new Ui::CSettingsFontDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        ui->comp_FontSettings->setMode(CSettingsFontComponent::GenerateQssOnly);

        connect(ui->comp_FontSettings, &CSettingsFontComponent::accept, this, &CSettingsFontDialog::accept);
        connect(ui->comp_FontSettings, &CSettingsFontComponent::reject, this, &CSettingsFontDialog::reject);
    }

    CSettingsFontDialog::~CSettingsFontDialog()
    {}

    const QString &CSettingsFontDialog::getQss() const
    {
        return ui->comp_FontSettings->getQss();
    }

    void CSettingsFontDialog::setFont(const QFont &font)
    {
        ui->comp_FontSettings->setFont(font);
    }

    QFont CSettingsFontDialog::getFont() const
    {
        return ui->comp_FontSettings->getFont();
    }

    QStringList CSettingsFontDialog::getFamilySizeStyle() const
    {
        return ui->comp_FontSettings->getFamilySizeStyle();
    }

    void CSettingsFontDialog::setWithColorSelection(bool withColor)
    {
        ui->comp_FontSettings->setWithColorSelection(withColor);
    }
} // ns
