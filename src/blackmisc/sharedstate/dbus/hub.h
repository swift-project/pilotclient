/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_DBUS_HUB_H
#define BLACKMISC_SHAREDSTATE_DBUS_HUB_H

#include "blackcore/corefacadeconfig.h"
#include "blackmisc/blackmiscexport.h"
#include <QSharedPointer>
#include <QObject>
#include <QFuture>

//! DBus interface for sharedstate hub.
#define BLACKMISC_HUB_INTERFACE "org.swift_project.blackmisc.sharedstate.hub"

//! DBus object path root for sharedstate hub.
#define BLACKMISC_HUB_PATH "/org/swift_project/hub"

class QDBusConnection;

namespace BlackMisc
{
    class CDBusServer;
    class CIdentifier;

    namespace SharedState::DBus
    {
        class IDuplex;

        /*!
         * Abstract interface for the hub in a star topology. An implementation detail of CDataLinkDBus.
         */
        class BLACKMISC_EXPORT IHub : public QObject
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKMISC_HUB_INTERFACE)

        public:
            //! Construct a new hub.
            static IHub *create(bool proxy, CDBusServer *server, const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

            //! Is connected?
            virtual bool isConnected() const = 0;

            //! Get a duplex object for the calling process.
            virtual std::pair<QSharedPointer<IDuplex>, QFuture<void>> getDuplex(const CIdentifier &) = 0;

        public slots:
            //! Create a duplex object for the identified process.
            virtual bool openDuplex(const BlackMisc::CIdentifier &client) = 0;

            //! Destroy the duplex object for the identified process.
            virtual void closeDuplex(const BlackMisc::CIdentifier &client) = 0;

        protected:
            //! Create a duplex object and return status via future.
            virtual QFuture<void> openDuplexAsync(const CIdentifier &client) = 0;

            //! Constructor.
            IHub(QObject *parent = nullptr);
        };
    }
}

#endif
