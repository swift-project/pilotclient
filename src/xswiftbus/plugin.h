/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_PLUGIN_H
#define BLACKSIM_XSWIFTBUS_PLUGIN_H

//! \file

/*!
 * \namespace XSwiftBus
 * Plugin loaded by X-Plane which publishes a DBus service
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "dbusconnection.h"
#include "menus.h"
#include <QObject>
#include <QVector>
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
    class CPlugin : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CPlugin();

        //! Destructor
        ~CPlugin();

        //! Called by XPluginReceiveMessage when the model is changed
        void onAircraftModelChanged();

        //! Called by XPluginReceiveMessage when the aircraft is positioned at an airport
        void onAircraftRepositioned();

    private:
        std::unique_ptr<CDBusConnection> m_dbusConnection;
        CService *m_service = nullptr;
        CTraffic *m_traffic = nullptr;
        CWeather *m_weather = nullptr;
        CMenu m_menu;
        CMenuItem m_startServerMenuItem;
        CMenuItem m_toggleMessageWindowMenuItem;

        std::thread m_dbusThread;
        bool m_shouldStop = false;

        void startServer(CDBusConnection::BusType bus);

        static float flightLoopCallback(float, float, int, void *refcon);
    };
}

#endif // guard
