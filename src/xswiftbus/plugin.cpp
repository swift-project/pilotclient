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
#include "utils.h"
#include "XPLM/XPLMProcessing.h"
#include <functional>
#include <thread>

namespace {
    inline std::string xswiftbusServiceName() {
        return std::string("org.swift-project.xswiftbus");
    }
}

namespace XSwiftBus
{

    CPlugin::CPlugin()
        : m_dbusConnection(std::make_unique<CDBusConnection>()), m_menu(CMenu::mainMenu().subMenu("XSwiftBus"))
    {
        m_startServerMenuItem = m_menu.item("Start XSwiftBus", [this]{ startServer(CDBusConnection::SessionBus); });
        m_toggleMessageWindowMenuItem = m_menu.item("Toggle Message Window", [this] { if(m_service) { m_service->toggleMessageBoxVisibility(); } });
        // m_startServerMenuItems.push_back(m_menu.item("Start server on system bus", [this]{ startServer(BlackMisc::CDBusServer::systemBusAddress()); }));
        // m_startServerMenuItems.push_back(m_menu.item("Start server on localhost P2P", [this]{ startServer(BlackMisc::CDBusServer::p2pAddress("localhost")); }));

        m_dbusThread = std::thread([this]()
        {
            while(!m_shouldStop)
            {
                m_dbusConnection->runBlockingEventLoop();
            }
        });

        XPLMRegisterFlightLoopCallback(flightLoopCallback, -1, this);
    }

    CPlugin::~CPlugin()
    {
        XPLMUnregisterFlightLoopCallback(flightLoopCallback, this);
        m_dbusConnection->close();
        m_shouldStop = true;
        if (m_dbusThread.joinable()) { m_dbusThread.join(); }
    }

    void CPlugin::startServer(CDBusConnection::BusType bus)
    {
        (void) bus;
        // for (auto &item : m_startServerMenuItems) { item.setEnabled(false); }
        m_startServerMenuItem.setEnabled(false);

        // Todo: retry if it fails
        bool success = m_dbusConnection->connect(CDBusConnection::SessionBus, xswiftbusServiceName());

        if (!success)
        {
            // Print error
            return;
        }

        m_service = new CService(m_dbusConnection.get());
        m_traffic = new CTraffic(m_dbusConnection.get());
        m_weather = new CWeather(m_dbusConnection.get());

        INFO_LOG("XSwiftBus started.");
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

    float CPlugin::flightLoopCallback(float, float, int, void *refcon)
    {
        auto *plugin = static_cast<CPlugin *>(refcon);
        if (plugin->m_service) { plugin->m_service->processDBus(); }
        if (plugin->m_weather) { plugin->m_weather->processDBus(); }
        if (plugin->m_traffic) { plugin->m_traffic->processDBus(); }
        return -1;
    }
}
