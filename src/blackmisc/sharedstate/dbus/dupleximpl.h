// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::postEvent
            virtual void postEvent(const QString &channel, const BlackMisc::CVariant &param) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::setSubscription
            virtual void setSubscription(const QString &channel, const BlackMisc::CVariantList &filters) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::requestPeerSubscriptions
            virtual void requestPeerSubscriptions() override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::submitRequest
            virtual void submitRequest(const QString &channel, const BlackMisc::CVariant &param, quint32 token) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::advertise
            virtual void advertise(const QString &channel) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::withdraw
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
