/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "atclistmgr.h"
#include "network.h"

BlackCore::CAtcListManager::CAtcListManager()
{
    INetwork *net = BlackMisc::IContext::getInstance().singleton<INetwork>();

    connect(net, &INetwork::atcPositionUpdate, this, &CAtcListManager::update, Qt::QueuedConnection);
    connect(net, &INetwork::atcDisconnected, this, &CAtcListManager::remove, Qt::QueuedConnection);
    connect(net, &INetwork::connectionStatusDisconnected, this, &CAtcListManager::clear, Qt::QueuedConnection);
}

void BlackCore::CAtcListManager::update(const QString& callsign, const BlackMisc::PhysicalQuantities::CFrequency& freq,
    const BlackMisc::Geo::CCoordinateGeodetic& pos, const BlackMisc::PhysicalQuantities::CLength& range)
{
    m_list.insert(BlackMisc::CAtcListEntry(callsign, freq, pos, range));
    emit listChanged(m_list);
}

void BlackCore::CAtcListManager::remove(const QString& callsign)
{
    m_list.remove(callsign);
    emit listChanged(m_list);
}

void BlackCore::CAtcListManager::clear()
{
    m_list.clear();
    emit listChanged(m_list);
}