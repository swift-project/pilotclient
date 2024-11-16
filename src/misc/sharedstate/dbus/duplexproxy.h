// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEXPROXY_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_DUPLEXPROXY_H

#include "misc/sharedstate/dbus/duplex.h"

namespace swift::misc
{
    class CGenericDBusInterface;

    namespace shared_state::dbus
    {
        /*!
         * Client side implementation of IDuplex, through which the client communicates with the server.
         */
        class SWIFT_MISC_EXPORT CDuplexProxy final : public IDuplex
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_DUPLEX_INTERFACE)

        public:
            //! Constructor.
            CDuplexProxy(const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

        public slots:
            //! \name Interface implementations
            //! @{

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::postEvent
            virtual void postEvent(const QString &channel, const swift::misc::CVariant &param) override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::setSubscription
            virtual void setSubscription(const QString &channel, const swift::misc::CVariantList &filters) override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::requestPeerSubscriptions
            virtual void requestPeerSubscriptions() override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::submitRequest
            virtual void submitRequest(const QString &channel, const swift::misc::CVariant &param, quint32 token) override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::reply
            virtual void reply(const swift::misc::CVariant &param, quint32 token) override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::advertise
            virtual void advertise(const QString &channel) override;

            //! \copydoc swift::misc::shared_state::DBus::IDuplex::withdraw
            virtual void withdraw(const QString &channel) override;
            //! @}

        private:
            CGenericDBusInterface *m_interface = nullptr;
        };
    } // namespace shared_state::dbus
} // namespace swift::misc

#endif
