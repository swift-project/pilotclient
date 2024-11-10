// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/jsonexception.h"
#include "misc/range.h"
#include <QStringBuilder>
#include <QThreadStorage>
#include <vector>

namespace swift::misc
{
    static QThreadStorage<std::vector<const CJsonScope *>> g_stack;

    //! \private
    auto &jsonStack() noexcept
    {
        return g_stack.localData();
    }

    // pin vtables to this file
    void CJsonException::anchor()
    {}

    QString CJsonException::toString(const QString &prefix) const
    {
        if (prefix.isEmpty()) { return QStringLiteral("%1 in '%2'").arg(what()).arg(getStackTrace()); }
        return QStringLiteral("%1: %2 in '%3'").arg(prefix).arg(what()).arg(getStackTrace());
    }

    QString CJsonException::stackString()
    {
        QStringList list;
        for (const auto scope : std::as_const(jsonStack()))
        {
            list.push_back(scope->m_string ? *scope->m_string : scope->m_latin1); // clazy:exclude=reserve-candidates
            if (scope->m_index >= 0) { list.back() += u'[' % QString::number(scope->m_index) % u']'; }
        }
        return list.isEmpty() ? QStringLiteral("<document root>") : list.join('.');
    }

    void CJsonScope::push() const noexcept
    {
        jsonStack().push_back(this);
    }

    void CJsonScope::pop() const noexcept
    {
        Q_ASSERT(jsonStack().back() == this);
        jsonStack().pop_back();
    }
}
