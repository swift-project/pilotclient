/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/connectionguard.h"

#include <utility> // for as_const

#include <QObject>

namespace BlackMisc
{
    CConnectionGuard::CConnectionGuard(const QMetaObject::Connection &connection)
    {
        m_connections.append(connection);
    }

    bool CConnectionGuard::append(const QMetaObject::Connection &connection)
    {
        if (!connection) { return false; }
        m_connections.append(connection);
        return true;
    }

    bool CConnectionGuard::append(const QList<QMetaObject::Connection> &connections)
    {
        int c = 0;
        for (const QMetaObject::Connection &connection : connections)
        {
            if (!connection) { continue; }
            if (this->append(connection)) { c++; }
        }
        return c > 0;
    }

    CConnectionGuard::~CConnectionGuard()
    {
        this->disconnectAll();
    }

    int CConnectionGuard::disconnectAll()
    {
        if (m_connections.isEmpty()) { return 0; }
        int c = 0;
        for (const QMetaObject::Connection &con : std::as_const(m_connections))
        {
            if (QObject::disconnect(con)) { c++; }
        }
        m_connections.clear();
        return c;
    }
} // ns
