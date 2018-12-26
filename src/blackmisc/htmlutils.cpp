/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "htmlutils.h"
#include <QStringBuilder>

namespace BlackMisc
{
    QString toHtmTable(const QStringList &values, int columns)
    {
        if (values.isEmpty() || columns < 1) { return {}; }

        QString html;
        const int size = values.size();
        for (int i = 0; i < size; i++)
        {
            html += QStringLiteral("<tr>");
            for (int c = 0; c < columns; c++)
            {
                const bool stillElements = (i < size);
                html += stillElements ?
                        u"<td>" % values.at(i) % u"</td>" :
                        QStringLiteral("<td></td>");
                if (c + 1 < columns) i++;
            }
            html += QStringLiteral("</tr>");
        }
        return u"<table>" % html % u"</table>";
    }

    QString unwrappedTooltip(const QString &text)
    {
        return u"<p style='white-space:pre'>" % text % u"</p>";
    }

} // ns
