// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleweatherdata

#include "reader.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/units.h"

#include <stdio.h>
#include <QByteArray>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QTextStream>
#include <QtGlobal>

using namespace BlackMisc::Geo;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

void CLineReader::run()
{
    QFile file;
    file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);

    forever
    {
        QString line = file.readLine().simplified();

        if (line == "x")
        {
            emit wantsToQuit();
            continue;
        }

        const QStringList parts = line.split(' ');
        if (parts.size() == 2)
        {
            const CLatitude latitude(CAngle::parsedFromString(parts.front(), CPqString::SeparatorBestGuess, CAngleUnit::deg()));
            const CLongitude longitude(CAngle::parsedFromString(parts.back(), CPqString::SeparatorBestGuess, CAngleUnit::deg()));
            const CAltitude alt(600, CLengthUnit::m());

            const CCoordinateGeodetic position { latitude, longitude, alt };
            emit weatherDataRequest(position);
        }
        else
        {
            QTextStream qtout(stdout);
            qtout << "Invalid command." << Qt::endl;
            qtout << "Usage: <lat> <lon>" << Qt::endl;
        }
    }
}
