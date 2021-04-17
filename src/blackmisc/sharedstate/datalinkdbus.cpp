/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/datalinkdbus.h"
#include "blackmisc/sharedstate/dbus/hub.h"
#include "blackmisc/sharedstate/dbus/duplex.h"
#include "blackmisc/sharedstate/dbus/duplexproxy.h"
#include "blackmisc/sharedstate/activeobserver.h"
#include "blackmisc/sharedstate/activemutator.h"
#include "blackmisc/dbusserver.h"
#include <QDBusServiceWatcher>
#include <QDBusConnection>

using namespace BlackMisc::SharedState::DBus;

namespace BlackMisc
{
    namespace SharedState
    {
        CDataLinkDBus::CDataLinkDBus(QObject *parent) : QObject(parent), m_watchTimer(this)
        {
            connect(&m_watchTimer, &QTimer::timeout, this, &CDataLinkDBus::checkConnection);
            m_watchTimer.setInterval(1000);
        }

        CDataLinkDBus::~CDataLinkDBus()
        {
            setConnectionStatus(false);
        }

        void CDataLinkDBus::overrideIdentifier(const CIdentifier &id)
        {
            m_identifier = id;
        }

        void CDataLinkDBus::initializeLocal(CDBusServer *server)
        {
            Q_ASSERT_X(!m_hub, Q_FUNC_INFO, "Already initialized");
            m_hub = IHub::create(false, server, QDBusConnection("unused"), {}, this);
            m_watchTimer.start();
            checkConnection();
        }

        void CDataLinkDBus::initializeRemote(const QDBusConnection &connection, const QString &service)
        {
            Q_ASSERT_X(!m_hub, Q_FUNC_INFO, "Already initialized");
            m_hub = IHub::create(true, nullptr, connection, service, this);
            m_watchTimer.start();
            checkConnection();
        }

        void CDataLinkDBus::checkConnection()
        {
            if (m_hub->isConnected()) { onConnected(); }
            else { onDisconnected(); }
        }

        void CDataLinkDBus::onConnected()
        {
            if (m_duplex) { return; }

            QFuture<void> ready;
            std::tie(m_duplex, ready) = m_hub->getDuplex(m_identifier);
            connect(m_duplex.get(), &IDuplex::eventPosted, this, &CDataLinkDBus::handlePeerEvent);
            connect(m_duplex.get(), &IDuplex::peerSubscriptionsReceived, this, &CDataLinkDBus::setPeerSubscriptions);
            connect(m_duplex.get(), &IDuplex::requestReceived, this, &CDataLinkDBus::handlePeerRequest);
            doAfter(ready, m_duplex.get(), [this]
            {
                m_duplex->requestPeerSubscriptions();
                announceLocalSubscriptions();
                setConnectionStatus(true);
            });
        }

        void CDataLinkDBus::onDisconnected()
        {
            m_duplex.reset();
            setConnectionStatus(false);
        }

        void CDataLinkDBus::handleLocalEvent(const QString &channel, const CVariant &param)
        {
            handlePeerEvent(channel, param);

            if (!m_duplex) { return; }

            for (const auto &filter : std::as_const(getChannel(channel).peerSubscriptions))
            {
                if (filter.matches(param))
                {
                    m_duplex->postEvent(channel, param);
                    return;
                }
            }
        }

        void CDataLinkDBus::handlePeerEvent(const QString &channel, const CVariant &param)
        {
            for (const auto &observerWeak : std::as_const(getChannel(channel).passiveObservers))
            {
                auto observer = observerWeak.lock();
                if (observer && observer->eventSubscription().matches(param))
                {
                    observer->handleEvent(param);
                }
            }
        }

        void CDataLinkDBus::announceLocalSubscriptions()
        {
            for (const auto &channel : getChannelNames())
            {
                announceLocalSubscriptions(channel);
            }
        }

        void CDataLinkDBus::announceLocalSubscriptions(const QString &channel)
        {
            CVariantList filters;
            for (const auto &observerWeak : std::as_const(getChannel(channel).passiveObservers))
            {
                auto observer = observerWeak.lock();
                if (observer) { filters.push_back(observer->eventSubscription()); }
            }
            m_duplex->setSubscription(channel, filters);
        }

        void CDataLinkDBus::setPeerSubscriptions(const QString &channel, const CVariantList &filters)
        {
            getChannel(channel).peerSubscriptions = filters;
        }

        QFuture<CVariant> CDataLinkDBus::handleLocalRequest(const QString &channel, const CVariant &param)
        {
            auto mutator = getChannel(channel).activeMutator.lock();
            if (mutator) { return mutator->handleRequest(param); }

            if (!m_duplex) { return {}; }

            return m_duplex->submitRequest(channel, param);
        }

        void CDataLinkDBus::handlePeerRequest(const QString &channel, const CVariant &param, quint32 token)
        {
            auto mutator = getChannel(channel).activeMutator.lock();
            if (mutator)
            {
                doAfter(mutator->handleRequest(param), this, [this, token](auto future)
                {
                    if (m_duplex) { m_duplex->reply(future.result(), token); }
                });
            }
        }

        void CDataLinkDBus::publish(const CPassiveMutator *mutator)
        {
            connect(mutator, &CPassiveMutator::eventPosted, this, [this, channel = getChannelName(mutator)](const CVariant &param)
            {
                handleLocalEvent(channel, param);
            });
        }

        void CDataLinkDBus::publish(const CActiveMutator *mutator)
        {
            publish(static_cast<const CPassiveMutator *>(mutator));

            auto &channel = getChannel(mutator);
            Q_ASSERT_X(! channel.activeMutator, Q_FUNC_INFO, "Tried to publish two active mutators on one channel");
            channel.activeMutator = mutator->weakRef();

            if (m_duplex)
            {
                m_duplex->advertise(getChannelName(mutator));
            }
            connect(mutator, &QObject::destroyed, this, [this, channel = getChannelName(mutator)]
            {
                if (m_duplex) { m_duplex->withdraw(channel); }
            });
        }

        void CDataLinkDBus::subscribe(const CPassiveObserver *observer)
        {
            getChannel(observer).passiveObservers.push_back(observer->weakRef());

            auto announce = [this, channel = getChannelName(observer)]
            {
                if (m_duplex) { announceLocalSubscriptions(channel); }
            };
            connect(observer, &CPassiveObserver::eventSubscriptionChanged, this, announce);
            connect(observer, &QObject::destroyed, this, announce);
            announce();
        }

        void CDataLinkDBus::subscribe(const CActiveObserver *observer)
        {
            subscribe(static_cast<const CPassiveObserver *>(observer));

            connect(observer, &CActiveObserver::requestPosted, this, [this, channel = getChannelName(observer)](const CVariant &param, CPromise<CVariant> reply)
            {
                reply.chainResult(handleLocalRequest(channel, param));
            });
        }

        QStringList CDataLinkDBus::getChannelNames() const
        {
            QMutexLocker lock(&m_channelsMutex);
            return m_channels.keys();
        }

        CDataLinkDBus::Channel &CDataLinkDBus::getChannel(const QString &name)
        {
            QMutexLocker lock(&m_channelsMutex);
            return m_channels[name];
        }

        CDataLinkDBus::Channel &CDataLinkDBus::getChannel(const QObject *object)
        {
            return getChannel(getChannelName(object));
        }
    }
}
