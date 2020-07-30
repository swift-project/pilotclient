/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

    namespace SharedState
    {
        namespace DBus
        {
            class CDuplex;

            /*!
             * Server side implementation of IHub. Maintains a collection of CDuplex objects.
             * \ingroup SharedState
             */
            class BLACKMISC_EXPORT CHub : public IHub
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
                virtual std::pair<QSharedPointer<IDuplex>, QFuture<void>> getDuplex(const CIdentifier &) override;

            public slots:
                //! \name Interface implementations
                //! @{
                virtual bool openDuplex(const BlackMisc::CIdentifier &client) override;
                virtual void closeDuplex(const BlackMisc::CIdentifier &client) override;
                //! @}

            protected:
                //! \name Interface implementations
                //! @{
                virtual QFuture<void> openDuplexAsync(const CIdentifier &client) override;
                //! @}

            private:
                CDBusServer *m_server = nullptr;
                QMap<CIdentifier, QSharedPointer<CDuplex>> m_clients;
            };
        }
    }
}

#endif
