// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEX_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEX_H

#include <QFuture>
#include <QMap>
#include <QObject>

#include "misc/genericdbusinterface.h"
#include "misc/promise.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

//! DBus interface for sharedstate hub.
#define SWIFT_MISC_DUPLEX_INTERFACE "org.swift_project.misc.sharedstate.duplex"

//! DBus object path root for sharedstate hub.
#define SWIFT_MISC_DUPLEX_PATH_ROOT "/org/swift_project/duplex"

namespace swift::misc
{
    class CVariantList;

    namespace shared_state::dbus
    {
        /*!
         * Abstract interface for the spoke in a star topology. An implementation detail of CDataLinkDBus.
         *
         * Signals send messages from server to client; slots send messages from client to server.
         */
        class SWIFT_MISC_EXPORT IDuplex : public QObject
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_DUPLEX_INTERFACE)

        public:
            //! Client submits a request to the server. Reply is returned via a future.
            QFuture<CVariant> submitRequest(const QString &channel, const swift::misc::CVariant &param);

            //! Server submits a request to the client. Reply is returned via a future.
            QFuture<CVariant> receiveRequest(const QString &channel, const swift::misc::CVariant &param);

        public slots:
            //! Client posts an event to the server.
            virtual void postEvent(const QString &channel, const swift::misc::CVariant &param) = 0;

            //! Client announces its subscription to an event channel.
            virtual void setSubscription(const QString &channel, const swift::misc::CVariantList &filters) = 0;

            //! Client requests to be notified of all other clients' subscriptions.
            virtual void requestPeerSubscriptions() = 0;

            //! \private Client submits a request to the server.
            virtual void submitRequest(const QString &channel, const swift::misc::CVariant &param, quint32 token) = 0;

            //! Client replies to a submitted request.
            virtual void reply(const swift::misc::CVariant &param, quint32 token);

            //! Client advertises that it can handle requests for the given channel.
            virtual void advertise(const QString &channel) = 0;

            //! Client advertises that it can no longer handle requests for the given channel.
            virtual void withdraw(const QString &channel) = 0;

        signals:
            //! Server has notified the client that an event has been posted.
            void eventPosted(const QString &channel, const swift::misc::CVariant &param);

            //! Server has notified the client that other clients' event subscriptions have changed.
            void peerSubscriptionsReceived(const QString &channel, const swift::misc::CVariantList &filters);

            //! Server has submitted a request to be handled by the client.
            void requestReceived(const QString &channel, const swift::misc::CVariant &param, quint32 token, QPrivateSignal);

            //! \private Server has relayed a reply to the client's request.
            void replyReceived(const QString &channel, const swift::misc::CVariant &param, quint32 token);

        protected:
            //! Constructor.
            IDuplex(QObject *parent = nullptr);

        private:
            quint32 getToken();

            quint32 m_token = 0;
            QMap<quint32, CPromise<CVariant>> m_submittedRequests;
            QMap<quint32, CPromise<CVariant>> m_receivedRequests;
        };
    } // namespace shared_state::dbus
} // namespace swift::misc

#endif
