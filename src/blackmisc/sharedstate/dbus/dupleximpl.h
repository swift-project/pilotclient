/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_DUPLEXIMPL_H
#define BLACKMISC_SHAREDSTATE_DBUS_DUPLEXIMPL_H

#include "blackmisc/sharedstate/dbus/duplex.h"
#include <functional>

namespace BlackMisc
{
    class CIdentifier;
    class CDBusServer;

    namespace SharedState::DBus
    {
        class CHub;

        /*!
         * Server side implementation of IDuplex. Receives messages from clients and forwards them to other clients via the CHub.
         */
        class BLACKMISC_EXPORT CDuplex final : public IDuplex
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKMISC_DUPLEX_INTERFACE)

        public:
            //! Constructor.
            CDuplex(CHub *hub, const CIdentifier &client, CDBusServer *server, QObject *parent = nullptr);

            //! Destructor.
            virtual ~CDuplex() override;

        public slots:
            //! \name Interface implementations
            //! @{
            virtual void postEvent(const QString &channel, const BlackMisc::CVariant &param) override;
            virtual void setSubscription(const QString &channel, const BlackMisc::CVariantList &filters) override;
            virtual void requestPeerSubscriptions() override;
            virtual void submitRequest(const QString &channel, const BlackMisc::CVariant &param, quint32 token) override;
            virtual void advertise(const QString &channel) override;
            virtual void withdraw(const QString &channel) override;
            //! @}

        private:
            void requestPeerSubscriptions(const QString &channel);

            CHub *m_hub = nullptr;
            QMap<QString, CVariantList> m_subscriptions;
            QSet<QString> m_handlingChannels;
        };
    }
}

#endif
