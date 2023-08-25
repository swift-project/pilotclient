// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sharedstate/dbus/dupleximpl.h"
#include "blackmisc/sharedstate/dbus/hubimpl.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/identifier.h"
#include "blackmisc/verify.h"

namespace BlackMisc::SharedState::DBus
{
    CDuplex::CDuplex(CHub *hub, const CIdentifier &client, CDBusServer *server, QObject *parent) : IDuplex(parent), m_hub(hub)
    {
        if (server) { server->addObject(client.toDBusObjectPath(BLACKMISC_DUPLEX_PATH_ROOT), this); }
    }

    CDuplex::~CDuplex()
    {
        while (!m_subscriptions.isEmpty())
        {
            const auto channel = m_subscriptions.firstKey(); // explicit copy because a reference would dangle
            setSubscription(channel, {});
        }
    }

    void CDuplex::postEvent(const QString &channel, const CVariant &param)
    {
        for (auto client : m_hub->clients())
        {
            if (client != this && client->m_subscriptions.value(channel).matches(param))
            {
                emit client->eventPosted(channel, param);
            }
        }
    }

    void CDuplex::setSubscription(const QString &channel, const CVariantList &filters)
    {
        if (filters.isEmpty()) { m_subscriptions.remove(channel); }
        else { m_subscriptions.insert(channel, filters); }

        for (auto client : m_hub->clients())
        {
            if (client != this) { client->requestPeerSubscriptions(channel); }
        }
    }

    void CDuplex::requestPeerSubscriptions()
    {
        QSet<QString> channels;
        for (auto client : m_hub->clients())
        {
            if (client != this) { channels.unite(client->m_subscriptions.keys().toSet()); }
        }
        for (const auto &channel : channels)
        {
            requestPeerSubscriptions(channel);
        }
    }

    void CDuplex::requestPeerSubscriptions(const QString &channel)
    {
        CVariantList filters;
        for (auto peer : m_hub->clients())
        {
            if (peer != this) { filters.push_back(peer->m_subscriptions.value(channel)); }
        }

        emit peerSubscriptionsReceived(channel, filters);
    }

    void CDuplex::submitRequest(const QString &channel, const CVariant &param, quint32 token)
    {
        for (auto handler : m_hub->clients())
        {
            if (handler != this && handler->m_handlingChannels.contains(channel))
            {
                doAfter(handler->receiveRequest(channel, param), this, [this, channel, token](QFuture<CVariant> future) {
                    emit this->replyReceived(channel, future.result(), token);
                });
                return;
            }
        }
    }

    void CDuplex::advertise(const QString &channel)
    {
        m_handlingChannels.insert(channel);
    }

    void CDuplex::withdraw(const QString &channel)
    {
        m_handlingChannels.remove(channel);
    }
}
