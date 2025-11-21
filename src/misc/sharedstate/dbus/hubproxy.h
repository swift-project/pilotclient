// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_HUBPROXY_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_HUBPROXY_H

#include <QDBusConnection>

#include "misc/sharedstate/dbus/hub.h"

namespace swift::misc
{
    class CGenericDBusInterface;

    namespace shared_state::dbus
    {
        /*!
         * Client side implementation of IHub.
         */
        class SWIFT_MISC_EXPORT CHubProxy final : public IHub
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_HUB_INTERFACE)

        public:
            //! Constructor.
            CHubProxy(const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

            //! Destructor.
            ~CHubProxy() override;

            //! \name Interface implementations
            //! @{
            bool isConnected() const override;
            std::pair<QSharedPointer<IDuplex>, QFuture<bool>> getDuplex(const CIdentifier &) override;
            //! @}

        public slots:
            //! \name Interface implementations
            //! @{

            //! \copydoc swift::misc::shared_state::dbus::IHub::openDuplex
            bool openDuplex(const swift::misc::CIdentifier &client) override;

            //! \copydoc swift::misc::shared_state::dbus::IHub::closeDuplex
            void closeDuplex(const swift::misc::CIdentifier &client) override;
            //! @}

        protected:
            //! \name Interface implementations
            //! @{
            QFuture<bool> openDuplexAsync(const CIdentifier &client) override;
            //! @}

        private:
            CGenericDBusInterface *m_interface = nullptr;
            QString m_service;
        };
    } // namespace shared_state::dbus
} // namespace swift::misc

#endif // SWIFT_MISC_SHAREDSTATE_DBUS_HUBPROXY_H
