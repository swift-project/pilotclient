// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_HUBPROXY_H
#define BLACKMISC_SHAREDSTATE_DBUS_HUBPROXY_H

#include <QDBusConnection>
#include "blackmisc/sharedstate/dbus/hub.h"

namespace BlackMisc
{
    class CGenericDBusInterface;

    namespace SharedState::DBus
    {
        /*!
         * Client side implementation of IHub.
         */
        class BLACKMISC_EXPORT CHubProxy final : public IHub
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKMISC_HUB_INTERFACE)

        public:
            //! Constructor.
            CHubProxy(const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

            //! Destructor.
            virtual ~CHubProxy() override;

            //! \name Interface implementations
            //! @{
            virtual bool isConnected() const override;
            virtual std::pair<QSharedPointer<IDuplex>, QFuture<void>> getDuplex(const CIdentifier &) override;
            //! @}

        public slots:
            //! \name Interface implementations
            //! @{

            //! \copydoc BlackMisc::SharedState::DBus::IHub::openDuplex
            virtual bool openDuplex(const BlackMisc::CIdentifier &client) override;

            //! \copydoc BlackMisc::SharedState::DBus::IHub::closeDuplex
            virtual void closeDuplex(const BlackMisc::CIdentifier &client) override;
            //! @}

        protected:
            //! \name Interface implementations
            //! @{
            virtual QFuture<void> openDuplexAsync(const CIdentifier &client) override;
            //! @}

        private:
            CGenericDBusInterface *m_interface = nullptr;
            QString m_service;
        };
    }
}

#endif
