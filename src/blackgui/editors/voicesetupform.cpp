// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "voicesetupform.h"
#include "ui_voicesetupform.h"
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;

namespace BlackGui::Editors
{
    CVoiceSetupForm::CVoiceSetupForm(QWidget *parent) : CForm(parent),
                                                        ui(new Ui::CVoiceSetupForm)
    {
        ui->setupUi(this);
        ui->cb_Override->setChecked(true);
        this->resetToDefaultValues();
        connect(ui->cb_Override, &QCheckBox::toggled, this, &CVoiceSetupForm::enabledToggled, Qt::QueuedConnection);
        connect(ui->pb_SetDefaults, &QPushButton::clicked, this, &CVoiceSetupForm::resetToDefaultValues);
    }

    CVoiceSetupForm::~CVoiceSetupForm()
    {}

    CVoiceSetup CVoiceSetupForm::getValue() const
    {
        const CVoiceSetup s(ui->le_AfvVoiceServerUrl->text(), ui->le_AfvMapUrl->text());
        return s;
    }

    const CVoiceSetup &CVoiceSetupForm::getDisabledValue() const
    {
        static const CVoiceSetup s;
        return s;
    }

    void CVoiceSetupForm::setValue(const CVoiceSetup &setup)
    {
        ui->le_AfvVoiceServerUrl->setText(setup.getAfvVoiceServerUrl());
        ui->le_AfvMapUrl->setText(setup.getAfvMapUrl());
    }

    bool CVoiceSetupForm::isVoiceSetupEnabled() const
    {
        return ui->cb_Override->isChecked();
    }

    void CVoiceSetupForm::setVoiceSetupEnabled(bool enabled)
    {
        ui->cb_Override->setChecked(enabled);
    }

    void CVoiceSetupForm::setAlwaysAllowOverride(bool allow)
    {
        m_alwaysAllowOverride = allow;
        if (allow)
        {
            ui->cb_Override->setEnabled(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_Override, false);
        }
    }

    void CVoiceSetupForm::showEnableInfo(bool visible)
    {
        m_visibleEnableInfo = visible;
        this->visibleEnableInfo(visible);
    }

    void CVoiceSetupForm::setReadOnly(bool readonly)
    {
        ui->pb_SetDefaults->setEnabled(!readonly);
        ui->le_AfvVoiceServerUrl->setReadOnly(readonly);
        ui->le_AfvMapUrl->setReadOnly(readonly);
        CGuiUtility::checkBoxesReadOnly(this, readonly);
        if (m_alwaysAllowOverride)
        {
            ui->cb_Override->setEnabled(true);
            CGuiUtility::checkBoxReadOnly(ui->cb_Override, false);
        }

        /**
        if (readonly && ui->cb_Override->isChecked())
        {
            // this is no value which will be stored
            ui->cb_Override->setChecked(false);
        }
        **/

        this->forceStyleSheetUpdate();
    }

    CStatusMessageList CVoiceSetupForm::validate(bool nested) const
    {
        Q_UNUSED(nested)
        const CVoiceSetup val(this->getValue());
        CStatusMessageList msgs(val.validate());
        if (this->isReadOnly())
        {
            // in readonly I cannot change the data anyway, so skip warnings
            msgs.removeWarningsAndBelow();
        }
        return msgs;
    }

    void CVoiceSetupForm::enabledToggled(bool enabled)
    {
        Q_UNUSED(enabled)
        this->setReadOnly(!enabled);
    }

    void CVoiceSetupForm::visibleEnableInfo(bool visible)
    {
        ui->cb_Override->setVisible(visible);
        ui->lbl_VoiceSetup->setVisible(visible);
        ui->pb_SetDefaults->setVisible(visible);
    }

    void CVoiceSetupForm::resetToDefaultValues()
    {
        CVoiceSetup s;
        this->setValue(s);
    }
} // ns
