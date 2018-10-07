/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsatcstationsinlinecomponent.h"
#include "ui_settingsatcstationsinlinecomponent.h"

#include <QTimer>
#include <QPointer>

using namespace BlackGui::Settings;

namespace BlackGui
{
    namespace Components
    {
        CSettingsAtcStationsInlineComponent::CSettingsAtcStationsInlineComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsAtcStationsInlineComponent)
        {
            ui->setupUi(this);
            connect(ui->rb_InRange, &QRadioButton::toggled, this, &CSettingsAtcStationsInlineComponent::changeSettings);
            connect(ui->cb_Frequency, &QRadioButton::released, this, &CSettingsAtcStationsInlineComponent::changeSettings);
            connect(ui->cb_VoiceRoom, &QRadioButton::released, this, &CSettingsAtcStationsInlineComponent::changeSettings);

            QPointer<CSettingsAtcStationsInlineComponent> myself(this);
            QTimer::singleShot(2000, this, [ = ]
            {
                if (!myself) { return; }
                this->onSettingsChanged();
            });
        }

        CSettingsAtcStationsInlineComponent::~CSettingsAtcStationsInlineComponent()
        { }

        void CSettingsAtcStationsInlineComponent::onSettingsChanged()
        {
            const CAtcStationsSettings s = m_atcSettings.getThreadLocal();
            ui->rb_InRange->setChecked(s.showOnlyInRange());
            ui->cb_Frequency->setChecked(s.showOnlyWithValidFrequency());
            ui->cb_VoiceRoom->setChecked(s.showOnlyWithValidVoiceRoom());
        }

        void CSettingsAtcStationsInlineComponent::changeSettings()
        {
            const bool onlyInRange = ui->rb_InRange->isChecked();
            const bool freq = ui->cb_Frequency->isChecked();
            const bool voice = ui->cb_VoiceRoom->isChecked();
            CAtcStationsSettings s = m_atcSettings.getThreadLocal();
            const CAtcStationsSettings oldSettings = s;
            s.setShowOnlyInRange(onlyInRange);
            s.setShowOnlyWithValidFrequency(freq);
            s.setShowOnlyWithValidVoiceRoom(voice);
            if (oldSettings == s) { return; }
            m_atcSettings.setAndSave(s);
            emit this->changed();
        }
    } // ns
} // ns
