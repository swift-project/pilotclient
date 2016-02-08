/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "tuple.h"

namespace BlackMisc
{

    TupleConverterBase::Parser::Parser(QString string)
    {
        string.remove(QRegExp("^\\s*\\(\\s*")); // remove first '('
        string.remove(QRegExp("\\s*\\)\\s*$")); // remove last ')'
        QString current;
        int level = 0;
        for (const auto c : string)
        {
            if (c == '(') { level++; }
            if (c == ')') { level--; }
            if (c == ',' && level == 0) { m_raw.push_back(current.trimmed()); current.clear(); }
            else { current += c; }
        }
        if (! current.trimmed().isEmpty()) { m_raw.push_back(current.trimmed()); current.clear(); }

        for (const auto &member : m_raw)
        {
            QRegExp simple("^o\\.(\\w+)$");
            if (member.contains(simple)) { m_names.push_back(simple.cap(1)); continue; }
            QRegExp meta("^attr\\s*\\(\\s*o\\.(\\w+)");
            if (member.contains(meta)) { m_names.push_back(meta.cap(1)); continue; }
            qFatal("BLACK_DECLARE_TUPLE_CONVERSION: Parser couldn't extract member name from \"%s\"", qPrintable(member));
        }
        for (auto &name : m_names) { name.remove(QRegExp("^m_")); }
    }

} // namespace
