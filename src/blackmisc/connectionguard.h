/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        CConnectionGuard &operator =(CConnectionGuard &&) = default;

        //! Not copyable
        //! @{
        CConnectionGuard(const CConnectionGuard &) = delete;
        CConnectionGuard &operator =(const CConnectionGuard &) = delete;
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
