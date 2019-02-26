/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DATALINKDBUS_H
#define BLACKMISC_SHAREDSTATE_DATALINKDBUS_H

#include "blackmisc/sharedstate/datalink.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/variantlist.h"
#include <QSharedPointer>

class QDBusConnection;

namespace BlackMisc
{
    class CDBusServer;

    namespace SharedState
    {
        namespace DBus
        {
            class IHub;
            class IDuplex;
        }

        /*!
         * A transport mechanism using signals and slots distributed by DBus.
         * \ingroup SharedState
         */
        class BLACKMISC_EXPORT CDataLinkDBus : public QObject, public IDataLink
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::SharedState::IDataLink)

        public:
            //! Constructor.
            CDataLinkDBus(QObject *parent = nullptr);

            //! Destructor.
            virtual ~CDataLinkDBus() override;

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
            DBus::IHub *m_hub = nullptr;
            QSharedPointer<DBus::IDuplex> m_duplex;
            QMap<QString, Channel> m_channels;
            mutable QMutex m_channelsMutex { QMutex::Recursive };
        };
    }
}

#endif
