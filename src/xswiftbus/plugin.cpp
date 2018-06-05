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
#include <cmath>
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
        m_planeViewSubMenu = m_menu.subMenu("Follow Plane View");
        planeViewOwnAircraftMenuItem = m_planeViewSubMenu.item("Own Aircraft", [this] { switchToOwnAircraftView(); });

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

        m_traffic->setPlaneViewMenu(m_planeViewSubMenu);

        INFO_LOG("XSwiftBus started.");
    }

    void CPlugin::switchToOwnAircraftView()
    {
        /* This is the hotkey callback.  First we simulate a joystick press and
         * release to put us in 'free view 1'.  This guarantees that no panels
         * are showing and we are an external view. */
        XPLMCommandButtonPress(xplm_joy_v_fr1);
        XPLMCommandButtonRelease(xplm_joy_v_fr1);

        /* Now we control the camera until the view changes. */
        XPLMControlCamera(xplm_ControlCameraUntilViewChanges, orbitOwnAircraftFunc, this);
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

    int CPlugin::orbitOwnAircraftFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon)
    {
        auto *plugin = static_cast<CPlugin *>(refcon);

        if (isLosingControl == 1) { return 0; }

        if (cameraPosition)
        {
            int w, h, x, y;
            // First get the screen size and mouse location. We will use this to decide
            // what part of the orbit we are in. The mouse will move us up-down and around.
            // fixme: In a future update, change the orbit only while right mouse button is pressed.
            XPLMGetScreenSize(&w, &h);
            XPLMGetMouseLocation(&x, &y);
            double heading = 360.0 * static_cast<double>(x) / static_cast<double>(w);
            double pitch = 20.0 * ((static_cast<double>(y) / static_cast<double>(h)) * 2.0 - 1.0);

            // Now calculate where the camera should be positioned to be 200
            // meters from the plane and pointing at the plane at the pitch and
            // heading we wanted above.
            static const double PI = std::acos(-1);
            double dx = -50.0 * sin(heading * PI / 180.0);
            double dz = 50.0 * cos(heading * PI / 180.0);
            double dy = -50.0 * tan(pitch * PI / 180.0);

            double lx, ly, lz;

            lx = plugin->m_ownAircraftPositionX.get();
            ly = plugin->m_ownAircraftPositionY.get();
            lz = plugin->m_ownAircraftPositionZ.get();

            // Fill out the camera position info.
            cameraPosition->x = static_cast<float>(lx + dx);
            cameraPosition->y = static_cast<float>(ly + dy);
            cameraPosition->z = static_cast<float>(lz + dz);
            cameraPosition->pitch = static_cast<float>(pitch);
            cameraPosition->heading = static_cast<float>(heading);
            cameraPosition->roll = 0;
        }

        // Return 1 to indicate we want to keep controlling the camera.
        return 1;
    }
}
