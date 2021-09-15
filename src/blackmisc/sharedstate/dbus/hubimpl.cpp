/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/dbus/hubimpl.h"
#include "blackmisc/sharedstate/dbus/dupleximpl.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/variantlist.h"

namespace BlackMisc::SharedState::DBus
{
    CHub::CHub(CDBusServer *server, QObject* parent) : IHub(parent), m_server(server)
    {
        if (server) { server->addObject(BLACKMISC_HUB_PATH, this); }
    }

    std::pair<QSharedPointer<IDuplex>, QFuture<void>> CHub::getDuplex(const CIdentifier &identifier)
    {
        auto future = openDuplexAsync(identifier);
        return std::make_pair(m_clients.value(identifier), future);
    }

    bool CHub::openDuplex(const BlackMisc::CIdentifier &client)
    {
        if (!m_clients.contains(client))
        {
            m_clients.insert(client, QSharedPointer<CDuplex>::create(this, client, m_server, this));
        }
        return true;
    }

    void CHub::closeDuplex(const BlackMisc::CIdentifier &client)
    {
        // Using take() instead of remove() because we need the
        // destruction to happen after the removal, not before.
        m_clients.take(client);
    }

    QFuture<void> CHub::openDuplexAsync(const CIdentifier &client)
    {
        openDuplex(client);

        CPromise<void> promise;
        promise.setResult();
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
