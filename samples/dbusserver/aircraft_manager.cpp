/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDebug>
#include "aircraft_manager.h"

CAircraftManager::CAircraftManager(QObject *parent) :
    QObject(parent)
{
}

CAircraftManager::~CAircraftManager()
{
}

QList<CRemoteAircraft> CAircraftManager::aircraftList() const
{
    return m_aircraftList;
}

void CAircraftManager::addAircraft(const CRemoteAircraft &aircraft)
{
    if (m_aircraftList.indexOf(aircraft) == -1)
    {
        qDebug() << "Got new aircraft from FSD with callsign: " << aircraft.getCallsign();
        m_aircraftList << aircraft;
    }
}
