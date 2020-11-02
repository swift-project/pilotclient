/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_DUPLEX_H
#define BLACKMISC_SHAREDSTATE_DBUS_DUPLEX_H

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/promise.h"
#include "blackmisc/variant.h"
#include <QObject>
#include <QFuture>
#include <QMap>

//! DBus interface for sharedstate hub.
#define BLACKMISC_DUPLEX_INTERFACE "org.swift_project.blackmisc.sharedstate.duplex"

//! DBus object path root for sharedstate hub.
#define BLACKMISC_DUPLEX_PATH_ROOT "/org/swift_project/duplex"

namespace BlackMisc
{
    class CVariantList;

    namespace SharedState
    {
        namespace DBus
        {
            /*!
             * Abstract interface for the spoke in a star topology. An implementation detail of CDataLinkDBus.
             *
             * Signals send messages from server to client; slots send messages from client to server.
             */
            class BLACKMISC_EXPORT IDuplex : public QObject
            {
                Q_OBJECT
                Q_CLASSINFO("D-Bus Interface", BLACKMISC_DUPLEX_INTERFACE)

            public:
                //! Client submits a request to the server. Reply is returned via a future.
                QFuture<CVariant> submitRequest(const QString &channel, const BlackMisc::CVariant &param);

                //! Server submits a request to the client. Reply is returned via a future.
                QFuture<CVariant> receiveRequest(const QString &channel, const BlackMisc::CVariant &param);

            public slots:
                //! Client posts an event to the server.
                virtual void postEvent(const QString &channel, const BlackMisc::CVariant &param) = 0;

                //! Client announces its subscription to an event channel.
                virtual void setSubscription(const QString &channel, const BlackMisc::CVariantList &filters) = 0;

                //! Client requests to be notified of all other clients' subscriptions.
                virtual void requestPeerSubscriptions() = 0;

                //! \private Client submits a request to the server.
                virtual void submitRequest(const QString &channel, const BlackMisc::CVariant &param, quint32 token) = 0;

                //! Client replies to a submitted request.
                virtual void reply(const BlackMisc::CVariant &param, quint32 token);

                //! Client advertises that it can handle requests for the given channel.
                virtual void advertise(const QString &channel) = 0;

                //! Client advertises that it can no longer handle requests for the given channel.
                virtual void withdraw(const QString &channel) = 0;

            signals:
                //! Server has notified the client that an event has been posted.
                void eventPosted(const QString &channel, const BlackMisc::CVariant &param);

                //! Server has notified the client that other clients' event subscriptions have changed.
                void peerSubscriptionsReceived(const QString &channel, const BlackMisc::CVariantList &filters);

                //! Server has submitted a request to be handled by the client.
                void requestReceived(const QString &channel, const BlackMisc::CVariant &param, quint32 token, QPrivateSignal);

                //! \private Server has relayed a reply to the client's request.
                void replyReceived(const QString &channel, const BlackMisc::CVariant &param, quint32 token);

            protected:
                //! Constructor.
                IDuplex(QObject *parent = nullptr);

            private:
                quint32 getToken();

                quint32 m_token = 0;
                QMap<quint32, CPromise<CVariant>> m_submittedRequests;
                QMap<quint32, CPromise<CVariant>> m_receivedRequests;
            };
        }
    }
}

#endif
