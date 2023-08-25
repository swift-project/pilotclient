// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_CONNECTIONGUARD_H
#define BLACKMISC_CONNECTIONGUARD_H

#include "blackmisc/blackmiscexport.h"

#include <QList>
#include <QMetaObject>

namespace BlackMisc
{
    /*!
     * List of QMetaObject::Connection. The trick here is that those connections will
     * be disconnected when the object is destroyed. So it can be used with lambdas or bind which feature
     * non QObjects, if those might be destroyed before the signaling QObject.
     */
    class BLACKMISC_EXPORT CConnectionGuard
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
} // ns

#endif
