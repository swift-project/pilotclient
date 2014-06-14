/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_XBUS_PLUGIN_H
#define BLACKSIM_XBUS_PLUGIN_H

//! \file

//! \namespace XBus

#define NOMINMAX
#include "menus.h"
#include "service.h"

#pragma push_macro("interface")
#undef interface
#include "blackcore/dbus_server.h"
#pragma pop_macro("interface")

#include <QVector>

namespace XBus
{
    /*!
     * Main plugin class
     */
    class CPlugin : public QObject
    {
        Q_OBJECT

    public:
        CPlugin();

        void onAircraftModelChanged();

    private:
        BlackCore::CDBusServer *m_server = nullptr;
        CService *m_service = nullptr;
        CMenu m_menu;
        QVector<CMenuItem> m_startServerMenuItems;

        void startServer(const QString &address);
    };
}

#endif // guard
