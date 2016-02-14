/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "plugin.h"
#include "service.h"
#include "traffic.h"
#include "weather.h"

namespace {
    inline QString xbusServiceName() {
        return QStringLiteral("org.swift-project.xbus");
    }
}

namespace XBus
{

    CPlugin::CPlugin()
        : m_menu(CMenu::mainMenu().subMenu("XBus"))
    {
        m_startServerMenuItems.push_back(m_menu.item("Start server on session bus", [this]{ startServer(BlackMisc::CDBusServer::sessionBusAddress()); }));
        m_startServerMenuItems.push_back(m_menu.item("Start server on system bus", [this]{ startServer(BlackMisc::CDBusServer::systemBusAddress()); }));
        m_startServerMenuItems.push_back(m_menu.item("Start server on localhost P2P", [this]{ startServer(BlackMisc::CDBusServer::p2pAddress("localhost")); }));
    }

    void CPlugin::startServer(const QString &address)
    {
        Q_ASSERT(! m_server);
        for (auto &item : m_startServerMenuItems) { item.setEnabled(false); }

        m_server = new BlackMisc::CDBusServer(xbusServiceName(), address, this);
        m_service = new CService(this);
        m_traffic = new CTraffic(this);
        m_weather = new CWeather(this);
        m_server->addObject(CService::ObjectPath(), m_service);
        m_server->addObject(CTraffic::ObjectPath(), m_traffic);
        m_server->addObject(CWeather::ObjectPath(), m_weather);
    }

    void CPlugin::onAircraftModelChanged()
    {
        if (m_service)
        {
            m_service->onAircraftModelChanged();
        }
    }

    void CPlugin::onAircraftRepositioned()
    {
        if (m_service)
        {
            m_service->updateAirportsInRange();
        }
    }

}
