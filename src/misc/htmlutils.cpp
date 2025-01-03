// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/htmlutils.h"

#include <QStringBuilder>
#include <QStringList>

namespace swift::misc
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
                html += stillElements ? u"<td>" % values.at(i) % u"</td>" : QStringLiteral("<td></td>");
                if (c + 1 < columns) i++;
            }
            html += QStringLiteral("</tr>");
        }
        return u"<table>" % html % u"</table>";
    }

    QString asHyperlink(const QString &url, const QString &text)
    {
        return text.isEmpty() ? u"<a href=\"" % url % u"\">" % url % u"</a>" :
                                u"<a href=\"" % url % u"\">" % text % u"</a>";
    }

} // namespace swift::misc
