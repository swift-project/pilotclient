// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/dbus/duplexproxy.h"

#include "misc/genericdbusinterface.h"
#include "misc/identifier.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state::dbus
{
    CDuplexProxy::CDuplexProxy(const QDBusConnection &connection, const QString &service, QObject *parent) : IDuplex(parent)
    {
        const QString path = CIdentifier::anonymous().toDBusObjectPath(SWIFT_MISC_DUPLEX_PATH_ROOT);
        m_interface = new CGenericDBusInterface(service, path, SWIFT_MISC_DUPLEX_INTERFACE, connection, this);
        m_interface->relayParentSignals();
    }

    void CDuplexProxy::postEvent(const QString &channel, const CVariant &param)
    {
        m_interface->callDBus(QLatin1String("postEvent"), channel, param);
    }

    void CDuplexProxy::setSubscription(const QString &channel, const CVariantList &filters)
    {
        m_interface->callDBus(QLatin1String("setSubscription"), channel, filters);
    }

    void CDuplexProxy::requestPeerSubscriptions()
    {
        m_interface->callDBus(QLatin1String("requestPeerSubscriptions"));
    }

    void CDuplexProxy::submitRequest(const QString &channel, const CVariant &param, quint32 token)
    {
        m_interface->callDBus(QLatin1String("submitRequest"), channel, param, token);
    }

    void CDuplexProxy::reply(const CVariant &param, quint32 token)
    {
        m_interface->callDBus(QLatin1String("reply"), param, token);
    }

    void CDuplexProxy::advertise(const QString &channel)
    {
        m_interface->callDBus(QLatin1String("advertise"), channel);
    }

    void CDuplexProxy::withdraw(const QString &channel)
    {
        m_interface->callDBus(QLatin1String("withdraw"), channel);
    }
} // namespace swift::misc::shared_state::dbus
