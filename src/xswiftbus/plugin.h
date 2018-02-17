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
#include "menus.h"

#pragma push_macro("interface")
#undef interface
#include "blackmisc/dbusserver.h"
#pragma pop_macro("interface")

#include <QVector>

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

        //! Called by XPluginReceiveMessage when the model is changed
        void onAircraftModelChanged();

        //! Called by XPluginReceiveMessage when the aircraft is positioned at an airport
        void onAircraftRepositioned();

    private:
        BlackMisc::CDBusServer *m_server = nullptr;
        CService *m_service = nullptr;
        CTraffic *m_traffic = nullptr;
        CWeather *m_weather = nullptr;
        CMenu m_menu;
        CMenuItem m_startServerMenuItem;
        CMenuItem m_toggleMessageWindowMenuItem;

        void startServer(const QString &address);
        void tryStartServer(const QString &address);
    };
}

#endif // guard
