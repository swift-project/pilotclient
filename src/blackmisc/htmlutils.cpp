/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/htmlutils.h"
#include <QStringBuilder>
#include <QStringList>

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

    QString asHyperlink(const QString &url, const QString &text)
    {
        return text.isEmpty() ?
                   u"<a href=\"" % url % u"\">" % url % u"</a>" :
                   u"<a href=\"" % url % u"\">" % text % u"</a>";
    }

} // ns
