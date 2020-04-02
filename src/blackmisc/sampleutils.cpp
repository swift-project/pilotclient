/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/sampleutils.h"

#include <QTextStream>
#include <QtGlobal>

namespace BlackMisc
{
    QString CSampleUtils::selectDirectory(const QStringList &directoryOptions, QTextStream &streamOut, QTextStream &streamIn)
    {
        Q_ASSERT(!directoryOptions.isEmpty());
        if (directoryOptions.isEmpty()) { return {}; }
        if (directoryOptions.size() < 2) { return directoryOptions.first(); }

        QString selectedDir;
        do
        {
            for (int i = 0; i < directoryOptions.size(); i++)
            {
                if (i > 0) { streamOut << "  "; }
                streamOut << (i + 1) << ") " << directoryOptions.at(i) ;
            }
            streamOut << Qt::endl;
            streamOut << "Select [1-" << directoryOptions.size() << "]: ";
            streamOut.flush();

            QString input = streamIn.readLine();
            bool ok;
            int s = input.trimmed().toInt(&ok);
            if (ok && s > 0 && s <= directoryOptions.size())
            {
                selectedDir = directoryOptions.at(s - 1);
                break;
            }
        }
        while (true);
        return selectedDir;
    }
}
