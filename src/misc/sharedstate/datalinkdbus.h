// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DATALINKDBUS_H
#define SWIFT_MISC_SHAREDSTATE_DATALINKDBUS_H

#include "misc/sharedstate/datalink.h"
#include "misc/swiftmiscexport.h"
#include "misc/variantlist.h"
#include "misc/identifier.h"
#include <QSharedPointer>

class QDBusConnection;

namespace swift::misc
{
    class CDBusServer;

    namespace shared_state
    {
        namespace dbus
        {
            class IHub;
            class IDuplex;
        }

        /*!
         * A transport mechanism using signals and slots distributed by DBus.
         * \ingroup shared_state
         */
        class SWIFT_MISC_EXPORT CDataLinkDBus : public QObject, public IDataLink
        {
            Q_OBJECT
            Q_INTERFACES(swift::misc::shared_state::IDataLink)

        public:
            //! Constructor.
            CDataLinkDBus(QObject *parent = nullptr);

            //! Destructor.
            virtual ~CDataLinkDBus() override;

            //! Override identifier for testing purposes.
            void overrideIdentifier(const CIdentifier &);

            //! Initialize on server side.
            void initializeLocal(CDBusServer *server = nullptr);

            //! Initialize on client side.
            void initializeRemote(const QDBusConnection &connection, const QString &service);

            virtual void publish(const CPassiveMutator *mutator) override;
            virtual void publish(const CActiveMutator *mutator) override;
            virtual void subscribe(const CPassiveObserver *observer) override;
            virtual void subscribe(const CActiveObserver *observer) override;

        private:
            struct Channel
            {
                QWeakPointer<const CActiveMutator> activeMutator;
                QVector<QWeakPointer<const CPassiveObserver>> passiveObservers;
                CVariantList peerSubscriptions;
            };
            QStringList getChannelNames() const;
            Channel &getChannel(const QString &name);
            Channel &getChannel(const QObject *object);

            void checkConnection();
            void onConnected();
            void onDisconnected();

            void handleLocalEvent(const QString &channel, const CVariant &param);
            void handlePeerEvent(const QString &channel, const CVariant &param);
            void announceLocalSubscriptions();
            void announceLocalSubscriptions(const QString &channel);
            void setPeerSubscriptions(const QString &channel, const CVariantList &filters);
            QFuture<CVariant> handleLocalRequest(const QString &channel, const CVariant &param);
            void handlePeerRequest(const QString &channel, const CVariant &param, quint32 token);

            QTimer m_watchTimer;
            dbus::IHub *m_hub = nullptr;
            QSharedPointer<dbus::IDuplex> m_duplex;
            CIdentifier m_identifier = CIdentifier::anonymous();

            QMap<QString, Channel> m_channels;
            mutable QRecursiveMutex m_channelsMutex;
        };
    }
}

#endif
