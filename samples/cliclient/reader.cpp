// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplecliclient

#include "reader.h"

#include <stdio.h>
#include <QByteArray>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QtGlobal>

namespace BlackSample
{
    void LineReader::run()
    {
        QFile file;
        file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
        forever
        {
            QString line = file.readLine().trimmed();
            if (!line.isEmpty())
            {
                emit command(line);
            }
        }
    }
}
