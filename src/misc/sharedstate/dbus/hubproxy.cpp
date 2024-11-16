// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/dbus/hubproxy.h"

#include "misc/genericdbusinterface.h"
#include "misc/identifier.h"
#include "misc/sharedstate/dbus/duplexproxy.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state::dbus
{
    CHubProxy::CHubProxy(const QDBusConnection &connection, const QString &service, QObject *parent) : IHub(parent), m_service(service)
    {
        m_interface = new CGenericDBusInterface(service, SWIFT_MISC_HUB_PATH, SWIFT_MISC_HUB_INTERFACE, connection, this);
        m_interface->relayParentSignals();
    }

    bool CHubProxy::isConnected() const
    {
        return m_interface->isValid();
    }

    std::pair<QSharedPointer<IDuplex>, QFuture<bool>> CHubProxy::getDuplex(const CIdentifier &identifier)
    {
        auto duplex = QSharedPointer<CDuplexProxy>::create(m_interface->connection(), m_service, this);
        connect(duplex.get(), &QObject::destroyed, this, [=] { closeDuplex(identifier); });
        return std::make_pair(duplex, openDuplexAsync(identifier));
    }

    bool CHubProxy::openDuplex(const CIdentifier &client)
    {
        return m_interface->callDBusRet<bool>(QLatin1String("openDuplex"), client);
    }

    void CHubProxy::closeDuplex(const CIdentifier &client)
    {
        m_interface->callDBus(QLatin1String("closeDuplex"), client);
    }

    QFuture<bool> CHubProxy::openDuplexAsync(const CIdentifier &client)
    {
        return m_interface->callDBusFuture<bool>(QLatin1String("openDuplex"), client);
    }

    CHubProxy::~CHubProxy()
    {
        closeDuplex(CIdentifier::anonymous());
    }
} // namespace swift::misc::shared_state::dbus
