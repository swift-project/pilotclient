// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_HUBIMPL_H
#define BLACKMISC_SHAREDSTATE_DBUS_HUBIMPL_H

#include "blackmisc/sharedstate/dbus/hub.h"
#include "blackmisc/identifier.h"
#include <QSharedPointer>
#include <QMap>

namespace BlackMisc
{
    class CDBusServer;

    namespace SharedState::DBus
    {
        class CDuplex;

        /*!
         * Server side implementation of IHub. Maintains a collection of CDuplex objects.
         */
        class BLACKMISC_EXPORT CHub final : public IHub
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKMISC_HUB_INTERFACE)

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

            //! \copydoc BlackMisc::SharedState::DBus::IHub::openDuplex
            virtual bool openDuplex(const BlackMisc::CIdentifier &client) override;

            //! \copydoc BlackMisc::SharedState::DBus::IHub::closeDuplex
            virtual void closeDuplex(const BlackMisc::CIdentifier &client) override;
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
