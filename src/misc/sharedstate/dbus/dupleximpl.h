// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEXIMPL_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEXIMPL_H

#include <functional>

#include "misc/sharedstate/dbus/duplex.h"

namespace swift::misc
{
    class CIdentifier;
    class CDBusServer;

    namespace shared_state::dbus
    {
        class CHub;

        /*!
         * Server side implementation of IDuplex. Receives messages from clients and forwards them to other clients via
         * the CHub.
         */
        class SWIFT_MISC_EXPORT CDuplex final : public IDuplex
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_DUPLEX_INTERFACE)

        public:
            //! Constructor.
            CDuplex(CHub *hub, const CIdentifier &client, CDBusServer *server, QObject *parent = nullptr);

            //! Destructor.
            virtual ~CDuplex() override;

        public slots:
            //! \name Interface implementations
            //! @{

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::postEvent
            virtual void postEvent(const QString &channel, const swift::misc::CVariant &param) override;

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::setSubscription
            virtual void setSubscription(const QString &channel, const swift::misc::CVariantList &filters) override;

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::requestPeerSubscriptions
            virtual void requestPeerSubscriptions() override;

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::submitRequest
            virtual void submitRequest(const QString &channel, const swift::misc::CVariant &param,
                                       quint32 token) override;

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::advertise
            virtual void advertise(const QString &channel) override;

            //! \copydoc swift::misc::shared_state::dbus::IDuplex::withdraw
            virtual void withdraw(const QString &channel) override;
            //! @}

        private:
            void requestPeerSubscriptions(const QString &channel);

            CHub *m_hub = nullptr;
            QMap<QString, CVariantList> m_subscriptions;
            QSet<QString> m_handlingChannels;
        };
    } // namespace shared_state::dbus
} // namespace swift::misc

#endif
