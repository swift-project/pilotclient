/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsviewupdatetimes.h"
#include "ui_settingsviewupdatetimes.h"

using namespace BlackGui::Settings;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui::Components
{
    CSettingsViewUpdateTimes::CSettingsViewUpdateTimes(QWidget *parent) : QFrame(parent),
                                                                          ui(new Ui::CSettingsViewUpdateTimes)
    {
        ui->setupUi(this);
        connect(ui->hs_RenderingUpdateTime, &QSlider::sliderReleased, this, &CSettingsViewUpdateTimes::ps_sliderChanged);
        connect(ui->hs_AircraftUpdateTime, &QSlider::sliderReleased, this, &CSettingsViewUpdateTimes::ps_sliderChanged);
        connect(ui->hs_AtcUpdateTime, &QSlider::sliderReleased, this, &CSettingsViewUpdateTimes::ps_sliderChanged);
        connect(ui->hs_UserUpdateTime, &QSlider::sliderReleased, this, &CSettingsViewUpdateTimes::ps_sliderChanged);
        connect(ui->tb_ResetUser, &QToolButton::clicked, this, &CSettingsViewUpdateTimes::ps_resetUpdateTimes);

        this->ps_settingsChanged();
    }

    CSettingsViewUpdateTimes::~CSettingsViewUpdateTimes()
    {}

    void CSettingsViewUpdateTimes::ps_sliderChanged()
    {
        CViewUpdateSettings settings;
        settings.setAircraftUpdateTime(CTime(ui->hs_AircraftUpdateTime->value(), CTimeUnit::s()));
        settings.setAtcUpdateTime(CTime(ui->hs_AtcUpdateTime->value(), CTimeUnit::s()));
        settings.setRenderingUpdateTime(CTime(ui->hs_RenderingUpdateTime->value(), CTimeUnit::s()));
        settings.setUserUpdateTime(CTime(ui->hs_UserUpdateTime->value(), CTimeUnit::s()));
        m_settings.setAndSave(settings);
    }

    void CSettingsViewUpdateTimes::ps_settingsChanged()
    {
        const CViewUpdateSettings settings(this->m_settings.get());
        ui->hs_AircraftUpdateTime->setValue(settings.getAircraftUpdateTime().toSeconds());
        ui->hs_AtcUpdateTime->setValue(settings.getAtcUpdateTime().toSeconds());
        ui->hs_RenderingUpdateTime->setValue(settings.getRenderingUpdateTime().toSeconds());
        ui->hs_UserUpdateTime->setValue(settings.getUserUpdateTime().toSeconds());
    }

    void CSettingsViewUpdateTimes::ps_resetUpdateTimes()
    {
        CViewUpdateSettings settings;
        m_settings.setAndSave(settings);
        this->ps_settingsChanged();
    }
} // ns
