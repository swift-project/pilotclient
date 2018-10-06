/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include <QStringBuilder>
#include "htmlutils.h"

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
                if (i < size)
                {
                    html += QStringLiteral("<td>") % values.at(i) % QStringLiteral("</td>");
                }
                else
                {
                    html += QStringLiteral("<td></td>");
                }
                i++;
            }
            html += QStringLiteral("</tr>");
        }
        return QStringLiteral("<table>") % html % QStringLiteral("</table>");
    }
} // ns
