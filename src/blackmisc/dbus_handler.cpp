/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/dbus_server.h"
#include "blackmisc/dbus_handler.h"

namespace BlackMisc
{

    IDBusHandler::IDBusHandler(QObject *parent) :
        QObject(parent), m_dbusserver(0), m_parent(parent)
    {
    }

    void IDBusHandler::setDBusServer(BlackCore::CDBusServer *dbusServer)
    {
        m_dbusserver = dbusServer;

        if (m_dbusserver)
            m_dbusserver->addObject(m_dbusPath, this);
    }

    void IDBusHandler::setDBusObjectPath(const QString &dbusPath)
    {
        m_dbusPath = dbusPath;
    }

} // namespace BlackMisc
