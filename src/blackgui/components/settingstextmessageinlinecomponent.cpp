/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "settingstextmessageinlinecomponent.h"
#include "ui_settingstextmessageinlinecomponent.h"

using namespace BlackGui::Settings;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CSettingsTextMessageInlineComponent::CSettingsTextMessageInlineComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsTextMessageInlineComponent)
        {
            ui->setupUi(this);
            connect(ui->cb_All, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
            connect(ui->cb_Frequency, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
            connect(ui->cb_Private, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
            connect(ui->cb_Supervisor, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
            settingsChanged();
        }

        CSettingsTextMessageInlineComponent::~CSettingsTextMessageInlineComponent()
        { }

        void CSettingsTextMessageInlineComponent::settingsChanged()
        {
            const CTextMessageSettings s(m_settings.get());
            ui->cb_All->setChecked(s.getPopupAllMessages());
            ui->cb_Supervisor->setChecked(s.getPopupSupervisorMessages());
            ui->cb_Frequency->setChecked(s.getPopupFrequencyMessages());
            ui->cb_Private->setChecked(s.getPopupPrivateMessages());
            ui->cb_Selcal->setChecked(s.getPopupSelcalMessages());
        }

        void CSettingsTextMessageInlineComponent::changeSettings()
        {
            CTextMessageSettings s(m_settings.get());
            s.setPopupAllMessages(ui->cb_All->isChecked());
            s.setPopupFrequencyMessages(ui->cb_Frequency->isChecked());
            s.setPopupPrivateMessages(ui->cb_Private->isChecked());
            s.setSupervisorMessages(ui->cb_Supervisor->isChecked());
            s.setPopupSelcalMessages(ui->cb_Selcal->isChecked());
            const CStatusMessage m = m_settings.setAndSave(s);
            CLogMessage::preformatted(m);
        }
    } // ns
} // ns
