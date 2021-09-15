/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
            virtual bool openDuplex(const BlackMisc::CIdentifier &client) override;
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
