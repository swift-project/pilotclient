// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_DBUS_HUB_H
#define SWIFT_MISC_SHAREDSTATE_DBUS_HUB_H

#include "core/corefacadeconfig.h"
#include "misc/swiftmiscexport.h"
#include <QSharedPointer>
#include <QObject>
#include <QFuture>

//! DBus interface for sharedstate hub.
#define SWIFT_MISC_HUB_INTERFACE "org.swift_project.misc.sharedstate.hub"

//! DBus object path root for sharedstate hub.
#define SWIFT_MISC_HUB_PATH "/org/swift_project/hub"

class QDBusConnection;

namespace swift::misc
{
    class CDBusServer;
    class CIdentifier;

    namespace shared_state::dbus
    {
        class IDuplex;

        /*!
         * Abstract interface for the hub in a star topology. An implementation detail of CDataLinkDBus.
         */
        class SWIFT_MISC_EXPORT IHub : public QObject
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_MISC_HUB_INTERFACE)

        public:
            //! Construct a new hub.
            static IHub *create(bool proxy, CDBusServer *server, const QDBusConnection &connection, const QString &service, QObject *parent = nullptr);

            //! Is connected?
            virtual bool isConnected() const = 0;

            //! Get a duplex object for the calling process.
            virtual std::pair<QSharedPointer<IDuplex>, QFuture<bool>> getDuplex(const CIdentifier &) = 0;

        public slots:
            //! Create a duplex object for the identified process.
            virtual bool openDuplex(const swift::misc::CIdentifier &client) = 0;

            //! Destroy the duplex object for the identified process.
            virtual void closeDuplex(const swift::misc::CIdentifier &client) = 0;

        protected:
            //! Create a duplex object and return status via future.
            virtual QFuture<bool> openDuplexAsync(const CIdentifier &client) = 0;

            //! Constructor.
            IHub(QObject *parent = nullptr);
        };
    }
}

#endif
