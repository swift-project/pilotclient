/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/dbus/hub.h"
#include "blackmisc/sharedstate/dbus/hubimpl.h"
#include "blackmisc/sharedstate/dbus/hubproxy.h"
#include <QDBusConnection>

namespace BlackMisc
{
    namespace SharedState
    {
        namespace DBus
        {
            IHub::IHub(QObject* parent) : QObject(parent)
            {}

            IHub* IHub::create(bool proxy, CDBusServer *server, const QDBusConnection &connection, const QString &service, QObject* parent)
            {
                if (proxy) { return new CHubProxy(connection, service, parent); }
                else { return new CHub(server, parent); }
            }
        }
    }
}
