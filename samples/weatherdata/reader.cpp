/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
using namespace BlackMisc::PhysicalQuantities;

void CLineReader::run()
{
    QFile file;
    file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
    QRegularExpression re("^(-?\\d+).([0,5])\\s(-?\\d+).([0,5])$");
    forever
    {
        QString line = file.readLine().trimmed();

        if (line == "x")
        {
            emit wantsToQuit();
            continue;
        }

        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch())
        {
            double latitudeValue = match.captured(1).toDouble();
            if (latitudeValue > 0) { latitudeValue += match.captured(2).toDouble() / 10; }
            else { { latitudeValue -= match.captured(2).toDouble() / 10; } }
            double longitudeValue = match.captured(3).toDouble();
            if (longitudeValue > 0) { longitudeValue += match.captured(4).toDouble() / 10; }
            else { longitudeValue -= match.captured(4).toDouble() / 10; }
            longitudeValue += match.captured(4).toDouble() / 10;
            const CLatitude latitude(latitudeValue, CAngleUnit::deg());
            const CLongitude longitude(longitudeValue, CAngleUnit::deg());
            const CCoordinateGeodetic position { latitude, longitude, {0} };
            emit weatherDataRequest(position);
        }
        else
        {
            QTextStream qtout(stdout);
            qtout << "Invalid command." << endl;
            qtout << "Usage: <lat> <lon>" << endl;
        }
    }
}
