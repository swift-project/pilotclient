// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "sampleutils.h"

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QtGlobal>

namespace swift::misc
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
                streamOut << (i + 1) << ") " << directoryOptions.at(i);
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
} // namespace swift::misc
