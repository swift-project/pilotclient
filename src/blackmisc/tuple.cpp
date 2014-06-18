/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
            if (c == ')') { level++; }
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
