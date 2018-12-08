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
        if (values.isEmpty() || columns < 1) { return QStringLiteral(""); }

        QString html;
        QString row;
        const int size = values.size();
        for (int i = 0; i < size; i++)
        {
            html += QStringLiteral("<tr>");
            for (int c = 0; c < columns; c++)
            {
                html += (i < size) ?
                        QStringLiteral("<td>") % values.at(i) % QStringLiteral("</td>") :
                        QStringLiteral("<td></td>");
                i++;
            }
            html += QStringLiteral("</tr>");
        }
        return QStringLiteral("<table>") % html % QStringLiteral("</table>");
    }

    QString unwrappedTooltip(const QString &text)
    {
        return QStringLiteral("<p style='white-space:pre'>") % text % QStringLiteral("</p>");
    }

} // ns
