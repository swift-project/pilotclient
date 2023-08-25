// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsatcstationsinlinecomponent.h"
#include "ui_settingsatcstationsinlinecomponent.h"

#include <QTimer>
#include <QPointer>
#include <QStringBuilder>

using namespace BlackGui::Settings;

namespace BlackGui::Components
{
    CSettingsAtcStationsInlineComponent::CSettingsAtcStationsInlineComponent(QWidget *parent) : QFrame(parent),
                                                                                                ui(new Ui::CSettingsAtcStationsInlineComponent)
    {
        ui->setupUi(this);
        ui->rb_All->setChecked(true); // default for AFV

        connect(ui->rb_InRange, &QRadioButton::toggled, this, &CSettingsAtcStationsInlineComponent::changeSettings, Qt::QueuedConnection);
        connect(ui->cb_Frequency, &QRadioButton::released, this, &CSettingsAtcStationsInlineComponent::changeSettings, Qt::QueuedConnection);

        QPointer<CSettingsAtcStationsInlineComponent> myself(this);
        QTimer::singleShot(2000, this, [=] {
            if (!myself) { return; }
            this->onSettingsChanged();
        });
    }

    CSettingsAtcStationsInlineComponent::~CSettingsAtcStationsInlineComponent()
    {}

    void CSettingsAtcStationsInlineComponent::setCounts(int all, int inRange)
    {
        static const QString sAll = ui->rb_All->text();
        static const QString sInRange = ui->rb_InRange->text();

        ui->rb_All->setText(all < 0 ? sAll : sAll % QStringLiteral(" (%1)").arg(all));
        ui->rb_InRange->setText(inRange < 0 ? sInRange : sInRange % QStringLiteral(" (%1)").arg(inRange));
    }

    void CSettingsAtcStationsInlineComponent::onSettingsChanged()
    {
        const CAtcStationsSettings s = m_atcSettings.getThreadLocal();
        if (s.showOnlyInRange())
        {
            ui->rb_InRange->setChecked(true);
        }
        else
        {
            ui->rb_All->setChecked(true);
        }

        ui->cb_Frequency->setChecked(s.showOnlyWithValidFrequency());
    }

    void CSettingsAtcStationsInlineComponent::changeSettings()
    {
        const bool onlyInRange = ui->rb_InRange->isChecked();
        const bool freq = ui->cb_Frequency->isChecked();
        CAtcStationsSettings s = m_atcSettings.getThreadLocal();
        const CAtcStationsSettings oldSettings = s;
        s.setShowOnlyInRange(onlyInRange);
        s.setShowOnlyWithValidFrequency(freq);
        if (oldSettings == s) { return; }
        m_atcSettings.setAndSave(s);
        emit this->changed();
    }
} // ns
