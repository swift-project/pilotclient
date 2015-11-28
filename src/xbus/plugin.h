/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_XBUS_PLUGIN_H
#define BLACKSIM_XBUS_PLUGIN_H

//! \file

/*!
 * \namespace XBus
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

namespace XBus
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
        QVector<CMenuItem> m_startServerMenuItems;

        void startServer(const QString &address);
    };
}

#endif // guard
