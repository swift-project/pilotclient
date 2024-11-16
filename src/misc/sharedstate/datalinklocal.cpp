// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/datalinklocal.h"

#include "misc/promise.h"
#include "misc/sharedstate/activemutator.h"
#include "misc/sharedstate/activeobserver.h"

namespace swift::misc::shared_state
{
    CDataLinkLocal::CDataLinkLocal(QObject *parent) : QObject(parent)
    {
        setConnectionStatus(true);
    }

    CDataLinkLocal::~CDataLinkLocal()
    {
        setConnectionStatus(false);
    }

    void CDataLinkLocal::publish(const CPassiveMutator *mutator)
    {
        connect(mutator, &CPassiveMutator::eventPosted, this, [this, channel = getChannelName(mutator)](const CVariant &param) { dispatchEvent(param, channel); });
    }

    void CDataLinkLocal::publish(const CActiveMutator *mutator)
    {
        publish(static_cast<const CPassiveMutator *>(mutator));

        auto &channel = getChannel(mutator);
        Q_ASSERT_X(!channel.activeMutator, Q_FUNC_INFO, "Tried to publish two active mutators on one channel");
        channel.activeMutator = mutator->weakRef();
    }

    void CDataLinkLocal::subscribe(const CPassiveObserver *observer)
    {
        getChannel(observer).passiveObservers.push_back(observer->weakRef());
    }

    void CDataLinkLocal::subscribe(const CActiveObserver *observer)
    {
        subscribe(static_cast<const CPassiveObserver *>(observer));

        connect(observer, &CActiveObserver::requestPosted, this, [this, channel = getChannelName(observer)](const CVariant &param, CPromise<CVariant> reply) {
            reply.chainResult(handleRequest(param, channel));
        });
    }

    void CDataLinkLocal::dispatchEvent(const CVariant &param, const QString &channel)
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

    QFuture<CVariant> CDataLinkLocal::handleRequest(const CVariant &param, const QString &channel)
    {
        auto mutator = getChannel(channel).activeMutator.lock();
        if (mutator) { return mutator->handleRequest(param); }
        return {};
    }

    CDataLinkLocal::Channel &CDataLinkLocal::getChannel(const QString &name)
    {
        QMutexLocker lock(&m_channelsMutex);
        return m_channels[name];
    }

    CDataLinkLocal::Channel &CDataLinkLocal::getChannel(const QObject *object)
    {
        return getChannel(getChannelName(object));
    }
} // namespace swift::misc::shared_state
