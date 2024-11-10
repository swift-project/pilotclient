// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/dbus/hub.h"
#include "misc/sharedstate/dbus/hubimpl.h"
#include "misc/sharedstate/dbus/hubproxy.h"
#include <QDBusConnection>

namespace swift::misc::shared_state::dbus
{
    IHub::IHub(QObject *parent) : QObject(parent)
    {}

    IHub *IHub::create(bool proxy, CDBusServer *server, const QDBusConnection &connection, const QString &service, QObject *parent)
    {
        if (proxy) { return new CHubProxy(connection, service, parent); }
        else { return new CHub(server, parent); }
    }
}
