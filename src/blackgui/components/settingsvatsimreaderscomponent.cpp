/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsvatsimreaderscomponent.h"
#include "ui_settingsvatsimreaderscomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;
using namespace BlackCore::Vatsim;

namespace BlackGui::Components
{
    CSettingsVatsimReadersComponent::CSettingsVatsimReadersComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CSettingsVatsimReadersComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsVatsimReadersComponent::save);
        connect(ui->pb_Reload, &QPushButton::clicked, this, &CSettingsVatsimReadersComponent::reload);
        this->initValues();
    }

    CSettingsVatsimReadersComponent::~CSettingsVatsimReadersComponent()
    { }

    void CSettingsVatsimReadersComponent::onSettingsChanged()
    {
        this->initValues();
    }

    void CSettingsVatsimReadersComponent::save()
    {
        const int metarSec = m_settingsMetars.get().getPeriodicTime().toMs() / 1000;
        const int bookingsSec = m_settingsBookings.get().getPeriodicTime().toMs() / 1000;
        const int dataFileSec = m_settingsDataFile.get().getPeriodicTime().toMs() / 1000;

        const int newMetarSec = ui->sb_Metar->value();
        if (newMetarSec != metarSec)
        {
            m_settingsMetars.setAndSaveProperty(CReaderSettings::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newMetarSec), CTimeUnit::s()}));
        }
        const int newBookingsSec = ui->sb_Bookings->value();
        if (newBookingsSec != bookingsSec)
        {
            m_settingsBookings.setAndSaveProperty(CReaderSettings::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newBookingsSec), CTimeUnit::s()}));
        }
        const int newDataFileSec = ui->sb_DataFile->value();
        if (newDataFileSec != dataFileSec)
        {
            m_settingsBookings.setAndSaveProperty(CReaderSettings::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newDataFileSec), CTimeUnit::s()}));
        }
    }

    void CSettingsVatsimReadersComponent::reload()
    {
        this->initValues();
    }

    void CSettingsVatsimReadersComponent::initValues()
    {
        const int metarSec = m_settingsMetars.get().getPeriodicTime().toMs() / 1000;
        const int bookingsSec = m_settingsBookings.get().getPeriodicTime().toMs() / 1000;
        const int dataFileSec = m_settingsDataFile.get().getPeriodicTime().toMs() / 1000;

        ui->sb_Metar->setValue(metarSec);
        ui->sb_Bookings->setValue(bookingsSec);
        ui->sb_DataFile->setValue(dataFileSec);
    }
} // ns
