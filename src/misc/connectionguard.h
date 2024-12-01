// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CONNECTIONGUARD_H
#define SWIFT_MISC_CONNECTIONGUARD_H

#include <QList>
#include <QMetaObject>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * List of QMetaObject::Connection. The trick here is that those connections will
     * be disconnected when the object is destroyed. So it can be used with lambdas or bind which feature
     * non QObjects, if those might be destroyed before the signaling QObject.
     */
    class SWIFT_MISC_EXPORT CConnectionGuard
    {
    public:
        //! Constructor
        CConnectionGuard() = default;

        //! Constructor
        CConnectionGuard(const QMetaObject::Connection &connection);

        //! Move constructor
        CConnectionGuard(CConnectionGuard &&) = default;

        //! Move assignment operator
        CConnectionGuard &operator=(CConnectionGuard &&) = default;

        //! @{
        //! Not copyable
        CConnectionGuard(const CConnectionGuard &) = delete;
        CConnectionGuard &operator=(const CConnectionGuard &) = delete;
        //! @}

        //! Destructor
        ~CConnectionGuard();

        //! Add connection
        bool append(const QMetaObject::Connection &connection);

        //! Add connections
        bool append(const QList<QMetaObject::Connection> &connections);

        //! Disconnect all
        int disconnectAll();

        //! Empty
        bool isEmpty() const { return m_connections.isEmpty(); }

    private:
        QList<QMetaObject::Connection> m_connections;
    };
} // namespace swift::misc

#endif // SWIFT_MISC_CONNECTIONGUARD_H
