// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/dbus/hubimpl.h"
#include "misc/sharedstate/dbus/dupleximpl.h"
#include "misc/dbusserver.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state::dbus
{
    CHub::CHub(CDBusServer *server, QObject *parent) : IHub(parent), m_server(server)
    {
        if (server) { server->addObject(SWIFT_MISC_HUB_PATH, this); }
    }

    std::pair<QSharedPointer<IDuplex>, QFuture<bool>> CHub::getDuplex(const CIdentifier &identifier)
    {
        auto future = openDuplexAsync(identifier);
        return std::make_pair(m_clients.value(identifier), future);
    }

    bool CHub::openDuplex(const swift::misc::CIdentifier &client)
    {
        if (!m_clients.contains(client))
        {
            m_clients.insert(client, QSharedPointer<CDuplex>::create(this, client, m_server, this));
        }
        return true;
    }

    void CHub::closeDuplex(const swift::misc::CIdentifier &client)
    {
        // Using take() instead of remove() because we need the
        // destruction to happen after the removal, not before.
        m_clients.take(client);
    }

    QFuture<bool> CHub::openDuplexAsync(const CIdentifier &client)
    {
        openDuplex(client);

        CPromise<bool> promise;
        promise.setResult(true);
        return promise.future();
    }

    CHub::~CHub()
    {
        // Disconnect clients from the hub before destroying them,
        // to avoid thrashing peer event subscription updates.
        const auto clients = std::move(m_clients);
        Q_UNUSED(clients)
    }
}
