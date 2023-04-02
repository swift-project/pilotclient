/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_DUPLEXPROXY_H
#define BLACKMISC_SHAREDSTATE_DBUS_DUPLEXPROXY_H

#include "blackmisc/sharedstate/dbus/duplex.h"

namespace BlackMisc
{
    class CGenericDBusInterface;

    namespace SharedState::DBus
    {
        /*!
         * Client side implementation of IDuplex, through which the client communicates with the server.
         */
        class BLACKMISC_EXPORT CDuplexProxy final : public IDuplex
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKMISC_DUPLEX_INTERFACE)

        public:
            //! Constructor.
            CDuplexProxy(const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

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

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::reply
            virtual void reply(const BlackMisc::CVariant &param, quint32 token) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::advertise
            virtual void advertise(const QString &channel) override;

            //! \copydoc BlackMisc::SharedState::DBus::IDuplex::withdraw
            virtual void withdraw(const QString &channel) override;
            //! @}

        private:
            CGenericDBusInterface *m_interface = nullptr;
        };
    }
}

#endif
