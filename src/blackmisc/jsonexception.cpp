/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/jsonexception.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include <QStringBuilder>
#include <QThreadStorage>
#include <vector>

namespace BlackMisc
{
    static QThreadStorage<std::vector<const CJsonScope *>> g_stack;

    //! \private
    auto &jsonStack() noexcept
    {
        return g_stack.localData();
    }

    // pin vtables to this file
    void CJsonException::anchor()
    { }

    CStatusMessage CJsonException::toStatusMessage(const CLogCategoryList &categories, const QString &prefix) const
    {
        return CStatusMessage(categories).validationError(toString(prefix));
    }

    QString CJsonException::toString(const QString &prefix) const
    {
        if (prefix.isEmpty()) { return QStringLiteral("%1 in '%2'").arg(what()).arg(getStackTrace()); }
        return QStringLiteral("%1: %2 in '%3'").arg(prefix).arg(what()).arg(getStackTrace());
    }

    void CJsonException::toLogMessage(const CLogCategoryList &categories, const QString &prefix) const
    {
        CLogMessage(categories).validationError(toString(prefix));
    }

    QString CJsonException::stackString()
    {
        QStringList list;
        for (const auto scope : BlackMisc::as_const(jsonStack()))
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
