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
        }

        CSettingsAtcStationsInlineComponent::~CSettingsAtcStationsInlineComponent()
        { }

        void CSettingsAtcStationsInlineComponent::onSettingsChanged()
        {
            const CAtcStationsSettings s = m_atcSettings.getThreadLocal();
            ui->rb_InRange->setChecked(s.showOnlyInRange());
        }

        void CSettingsAtcStationsInlineComponent::changeSettings()
        {
            const bool onlyInRange = ui->rb_InRange->isChecked();
            CAtcStationsSettings s = m_atcSettings.getThreadLocal();
            if (s.showOnlyInRange() && onlyInRange) { return; }
            s.setShowOnlyInRange(onlyInRange);
            m_atcSettings.setAndSave(s);

            emit this->changed();
        }
    } // ns
} // ns
