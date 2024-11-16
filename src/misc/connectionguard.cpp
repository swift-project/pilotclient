// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/connectionguard.h"

#include <utility> // for as_const

#include <QObject>

namespace swift::misc
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
} // namespace swift::misc
