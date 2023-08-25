// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "plugin.h"
#include "service.h"
#include "traffic.h"
#include "weather.h"
#include "utils.h"
#include "XPLM/XPLMProcessing.h"
#include <cmath>
#include <functional>
#include <thread>

namespace
{
    inline std::string xswiftbusServiceName()
    {
        return std::string("org.swift-project.xswiftbus");
    }
}

namespace XSwiftBus
{
    CPlugin::CPlugin()
        : m_dbusConnection(std::make_shared<CDBusConnection>()), m_menu(CMenu::mainMenu().subMenu("XSwiftBus"))
    {
        // m_startServerMenuItem = m_menu.item("Start XSwiftBus", [this]{ startServer(CDBusConnection::SessionBus); });

        m_showHideLabelsMenuItem = m_menu.item("Show/Hide Aircraft Labels", [this] {
            m_traffic->setDrawingLabels(!m_traffic->isDrawingLabels());
        });
        m_enableDisableXPlaneAtisMenuItem = m_menu.item("Enable/Disable X-Plane ATIS", [this] {
            m_atisEnabled.set(m_atisEnabled.get() ? 0 : 1);
        });
        m_messageWindowSubMenu = m_menu.subMenu("Message Window");
        m_toggleMessageWindowMenuItem = m_messageWindowSubMenu.item("Show/Hide", [this] {
            m_service->toggleMessageBoxVisibility();
        });
        m_popupMessageWindowMenuItem = m_messageWindowSubMenu.checkableItem("Pop up Window on new Message", true, [this](bool checked) {
            m_popupMessageWindowMenuItem.setChecked(!checked);
            m_service->setPopupMessageWindow(!checked);
        });
        m_disappearMessageWindowMenuItem = m_messageWindowSubMenu.checkableItem("Hide Message Window automatically", true, [this](bool checked) {
            m_disappearMessageWindowMenuItem.setChecked(!checked);
            m_service->setDisappearMessageWindow(!checked);
        });
        m_planeViewSubMenu = m_menu.subMenu("Follow Plane View");
        m_planeViewOwnAircraftMenuItem = m_planeViewSubMenu.item("Own Aircraft", [this] {
            m_traffic->setFollowedAircraft(CTraffic::ownAircraftString());
        });

        /*m_dbusThread = std::thread([this]()
        {
            while(!m_shouldStop)
            {
                m_dbusConnection->runBlockingEventLoop();
            }
        });*/

        // Delay the start of XSwiftBus.
        // http://www.xsquawkbox.net/xpsdk/mediawiki/DeferredInitialization
        XPLMRegisterFlightLoopCallback(startServerDeferred, -1, this);

        m_atisSaved = m_atisEnabled.get();
        m_atisEnabled.set(0);
    }

    CPlugin::~CPlugin()
    {
        m_atisEnabled.set(m_atisSaved);

        XPLMUnregisterFlightLoopCallback(flightLoopCallback, this);
        m_dbusConnection->close();
        m_shouldStop = true;
        if (m_dbusThread.joinable()) { m_dbusThread.join(); }
    }

    void CPlugin::readConfig()
    {
        initXPlanePath();
        const std::string configFilePath = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "xswiftbus.conf";
        m_pluginConfig.setFilePath(configFilePath);
        m_pluginConfig.parse();
        m_pluginConfig.print();
    }

    void CPlugin::startServer()
    {
        XPLMRegisterFlightLoopCallback(flightLoopCallback, -1, this);

        readConfig();

        m_service = std::make_unique<CService>(this);
        m_traffic = std::make_unique<CTraffic>(this);
        m_weather = std::make_unique<CWeather>(this);

        m_traffic->setPlaneViewMenu(m_planeViewSubMenu);

        if (m_pluginConfig.getDBusMode() == CConfig::DBusP2P)
        {
            m_dbusP2PServer = std::make_unique<CDBusServer>();

            // FIXME: make listen address configurable
            std::string listenAddress = "tcp:host=" + m_pluginConfig.getDBusAddress() + ",port=" + std::to_string(m_pluginConfig.getDBusPort());
            if (!m_dbusP2PServer->listen(listenAddress))
            {
                m_service->addTextMessage("XSwiftBus startup failed!", 255, 0, 0);
                return;
            }
            m_dbusP2PServer->setDispatcher(&m_dbusDispatcher);

            m_dbusP2PServer->setNewConnectionFunc([this](const std::shared_ptr<CDBusConnection> &conn) {
                m_dbusConnection = conn;
                m_dbusConnection->setDispatcher(&m_dbusDispatcher);
                m_service->setDBusConnection(m_dbusConnection);
                m_service->registerDBusObjectPath(m_service->InterfaceName(), m_service->ObjectPath());
                m_traffic->setDBusConnection(m_dbusConnection);
                m_traffic->registerDBusObjectPath(m_traffic->InterfaceName(), m_traffic->ObjectPath());
                m_weather->setDBusConnection(m_dbusConnection);
                m_weather->registerDBusObjectPath(m_weather->InterfaceName(), m_weather->ObjectPath());
            });
        }
        else
        {
            // Todo: retry if it fails
            bool success = m_dbusConnection->connect(CDBusConnection::SessionBus);

            if (!success)
            {
                // Print error
                return;
            }

            m_dbusConnection->setDispatcher(&m_dbusDispatcher);
            m_dbusConnection->requestName(xswiftbusServiceName());

            m_service->setDBusConnection(m_dbusConnection);
            m_service->registerDBusObjectPath(m_service->InterfaceName(), m_service->ObjectPath());
            m_traffic->setDBusConnection(m_dbusConnection);
            m_traffic->registerDBusObjectPath(m_traffic->InterfaceName(), m_traffic->ObjectPath());
            m_weather->setDBusConnection(m_dbusConnection);
            m_weather->registerDBusObjectPath(m_weather->InterfaceName(), m_weather->ObjectPath());
        }

        //! todo RR: Send all logs to the the message window.
        const std::string msg = "XSwiftBus " + m_service->getVersionNumber() + " started.";
        INFO_LOG(msg);
        m_service->addTextMessage(msg, 0, 255, 255);
    }

    void CPlugin::onAircraftModelChanged()
    {
        if (m_service)
        {
            m_service->onAircraftModelChanged();
            m_service->resetFrameTotals();
        }
    }

    void CPlugin::onAircraftRepositioned()
    {
        if (m_service)
        {
            m_service->updateAirportsInRange();
            m_service->resetFrameTotals();
        }
    }

    void CPlugin::onSceneryLoaded()
    {
        if (m_service)
        {
            m_service->onSceneryLoaded();
        }
    }

    float CPlugin::startServerDeferred(float, float, int, void *refcon)
    {
        auto *plugin = static_cast<CPlugin *>(refcon);
        if (!plugin->m_isRunning)
        {
            plugin->startServer();
            plugin->m_isRunning = true;
            INFO_LOG("XSwiftBus plugin started (deferred)");
        }
        return 0;
    }

    float CPlugin::flightLoopCallback(float, float, int, void *refcon)
    {
        auto *plugin = static_cast<CPlugin *>(refcon);

        plugin->m_dbusDispatcher.runOnce();
        if (plugin->m_service) { plugin->m_service->process(); }
        if (plugin->m_weather) { plugin->m_weather->process(); }
        if (plugin->m_traffic) { plugin->m_traffic->process(); }
        return -1;
    }
}
