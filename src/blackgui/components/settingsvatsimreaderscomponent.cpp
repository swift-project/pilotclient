/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsvatsimreaderscomponent.h"
#include "ui_settingsvatsimreaderscomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;
using namespace BlackCore::Settings;

namespace BlackGui
{
    namespace Components
    {
        CSettingsVatsimReadersComponent::CSettingsVatsimReadersComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsVatsimReadersComponent)
        {
            ui->setupUi(this);
            connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsVatsimReadersComponent::ps_save);
            connect(ui->pb_Reload, &QPushButton::clicked, this, &CSettingsVatsimReadersComponent::ps_reload);
            this->initValues();
        }

        CSettingsVatsimReadersComponent::~CSettingsVatsimReadersComponent()
        { }

        void CSettingsVatsimReadersComponent::ps_settingsChanged()
        {
            this->initValues();
        }

        void CSettingsVatsimReadersComponent::ps_save()
        {
            const int metarSec = this->m_settingsMetars.get().getPeriodicTime().toMs() / 1000;
            const int bookingsSec = this->m_settingsBookings.get().getPeriodicTime().toMs() / 1000;
            const int dataFileSec = this->m_settingsDataFile.get().getPeriodicTime().toMs() / 1000;

            const int newMetarSec = ui->sb_Metar->value();
            if (newMetarSec != metarSec)
            {
                this->m_settingsMetars.setAndSaveProperty(CSettingsReader::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newMetarSec), CTimeUnit::s()}));
            }
            const int newBookingsSec = ui->sb_Bookings->value();
            if (newBookingsSec != bookingsSec)
            {
                this->m_settingsBookings.setAndSaveProperty(CSettingsReader::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newBookingsSec), CTimeUnit::s()}));
            }
            const int newDataFileSec = ui->sb_DataFile->value();
            if (newDataFileSec != dataFileSec)
            {
                this->m_settingsBookings.setAndSaveProperty(CSettingsReader::IndexPeriodicTime, CVariant::fromValue(CTime{static_cast<double>(newDataFileSec), CTimeUnit::s()}));
            }
        }

        void CSettingsVatsimReadersComponent::ps_reload()
        {
            this->initValues();
        }

        void CSettingsVatsimReadersComponent::initValues()
        {
            const int metarSec = this->m_settingsMetars.get().getPeriodicTime().toMs() / 1000;
            const int bookingsSec = this->m_settingsBookings.get().getPeriodicTime().toMs() / 1000;
            const int dataFileSec = this->m_settingsDataFile.get().getPeriodicTime().toMs() / 1000;

            ui->sb_Metar->setValue(metarSec);
            ui->sb_Bookings->setValue(bookingsSec);
            ui->sb_DataFile->setValue(dataFileSec);
        }
    } // ns
} // ns
