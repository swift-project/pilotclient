// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKSIM_XSWIFTBUS_PLUGIN_H
#define BLACKSIM_XSWIFTBUS_PLUGIN_H

//! \file

/*!
 * \namespace XSwiftBus
 * Plugin loaded by X-Plane which publishes a DBus service
 */

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#include "dbusconnection.h"
#include "dbusdispatcher.h"
#include "dbusserver.h"
#include "datarefs.h"
#include "menus.h"
#include "config.h"
#include "settings.h"

#include "XPLM/XPLMCamera.h"
#include <memory>
#include <thread>

namespace XSwiftBus
{
    class CService;
    class CTraffic;
    class CWeather;

    /*!
     * Main plugin class
     */
    class CPlugin final : public CSettingsProvider
    {
    public:
        //! Constructor
        CPlugin();

        //! Destructor
        ~CPlugin();

        //! Called by XPluginReceiveMessage when the model is changed
        void onAircraftModelChanged();

        //! Called by XPluginReceiveMessage when the aircraft is positioned at an airport
        void onAircraftRepositioned();

        //! Called by XPluginReceiveMessage when some scenery is loaded
        void onSceneryLoaded();

        //! Is running
        bool isRunning() const { return m_isRunning; }

        //! Should stop
        bool shouldStop() const { return m_shouldStop; }

        //! \copydoc XSwiftBus::CSettingsProvider::getConfig
        virtual const CConfig &getConfig() const override { return m_pluginConfig; }

        //! \copydoc XSwiftBus::CSettingsProvider::writeConfig
        virtual bool writeConfig(bool tcas, bool debug) override { return m_pluginConfig.writeConfig(tcas, debug); }

    private:
        CConfig m_pluginConfig;
        CDBusDispatcher m_dbusDispatcher;
        std::unique_ptr<CDBusServer> m_dbusP2PServer;
        std::shared_ptr<CDBusConnection> m_dbusConnection;
        std::unique_ptr<CService> m_service;
        std::unique_ptr<CTraffic> m_traffic;
        std::unique_ptr<CWeather> m_weather;
        CMenu m_menu;
        CMenuItem m_startServerMenuItem;
        CMenuItem m_showHideLabelsMenuItem;
        CMenuItem m_enableDisableXPlaneAtisMenuItem;
        CMenu m_messageWindowSubMenu;
        CMenuItem m_toggleMessageWindowMenuItem;
        CMenuItem m_popupMessageWindowMenuItem;
        CMenuItem m_disappearMessageWindowMenuItem;
        CMenu m_planeViewSubMenu;
        CMenuItem m_planeViewOwnAircraftMenuItem;

        DataRef<xplane::data::sim::atc::atis_enabled> m_atisEnabled;
        decltype(m_atisEnabled.get()) m_atisSaved = 0;

        std::thread m_dbusThread;
        bool m_isRunning = false;
        bool m_shouldStop = false;

        void readConfig();
        void startServer();

        static float startServerDeferred(float, float, int, void *refcon);
        static float flightLoopCallback(float, float, int, void *refcon);
    };
} // ns

#endif // guard
