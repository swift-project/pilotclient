// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_HUBIMPL_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_HUBIMPL_H

#include "misc/sharedstate/dbus/hub.h"
#include "misc/identifier.h"
#include <QSharedPointer>
#include <QMap>

namespace swift::misc
{
    class CDBusServer;

    namespace shared_state::dbus
    {
        class CDuplex;

        /*!
         * Server side implementation of IHub. Maintains a collection of CDuplex objects.
         */
        class SWIFT_MISC_EXPORT CHub final : public IHub
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_HUB_INTERFACE)

        public:
            //! Constructor.
            CHub(CDBusServer *server, QObject *parent = nullptr);

            //! Destructor.
            virtual ~CHub() override;

            //! Returns a range containing all duplex objects.
            const auto &clients() const { return m_clients; }

            //! \name Interface implementations
            //! @{
            virtual bool isConnected() const override { return true; }
            virtual std::pair<QSharedPointer<IDuplex>, QFuture<bool>> getDuplex(const CIdentifier &) override;
            //! @}

        public slots:
            //! \name Interface implementations
            //! @{

            //! \copydoc swift::misc::shared_state::DBus::IHub::openDuplex
            virtual bool openDuplex(const swift::misc::CIdentifier &client) override;

            //! \copydoc swift::misc::shared_state::DBus::IHub::closeDuplex
            virtual void closeDuplex(const swift::misc::CIdentifier &client) override;
            //! @}

        protected:
            //! \name Interface implementations
            //! @{
            virtual QFuture<bool> openDuplexAsync(const CIdentifier &client) override;
            //! @}

        private:
            CDBusServer *m_server = nullptr;
            QMap<CIdentifier, QSharedPointer<CDuplex>> m_clients;
        };
    }
}

#endif
