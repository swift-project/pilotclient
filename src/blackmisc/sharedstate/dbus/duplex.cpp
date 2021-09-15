/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/dbus/duplex.h"

namespace BlackMisc::SharedState::DBus
{
    IDuplex::IDuplex(QObject* parent) : QObject(parent)
    {
        connect(this, &IDuplex::replyReceived, this, [this](const QString &, const CVariant &param, quint32 token)
        {
            const auto it = m_submittedRequests.find(token);
            if (it == m_submittedRequests.end()) { return; }
            it->setResult(param);
            m_submittedRequests.erase(it);
        });
    }

    QFuture<CVariant> IDuplex::submitRequest(const QString &channel, const CVariant &param)
    {
        const auto token = getToken();
        auto future = m_submittedRequests.insert(token, {})->future();
        submitRequest(channel, param, token);
        return future;
    }

    QFuture<CVariant> IDuplex::receiveRequest(const QString &channel, const BlackMisc::CVariant &param)
    {
        const auto token = getToken();
        auto future = m_receivedRequests.insert(token, {})->future();
        emit requestReceived(channel, param, token, {});
        return future;
    }

    void IDuplex::reply(const BlackMisc::CVariant &param, quint32 token)
    {
        const auto it = m_receivedRequests.find(token);
        if (it == m_receivedRequests.end()) { return; }
        it->setResult(param);
        m_receivedRequests.erase(it);
    }

    quint32 IDuplex::getToken()
    {
        return m_token++;
    }
}
