/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesgeodetictoecef.h"
#include <QElapsedTimer>

using namespace BlackMisc::Geo;

namespace BlackMiscTest
{

/*
 * Samples
 */
int CSamplesGeodeticToEcef::samples()
{

    QElapsedTimer timer;
    qint64 duration;

    double lat = 27.999999, lon = 86.999999, h = 8820.999999; // Mt Everest
    CCoordinateGeodetic startVec(lat, lon, h);
    std::cout << startVec << std::endl;

    timer.start();
    CCoordinateEcef mediumvec = CCoordinateTransformation::toEcef(startVec);
    duration = timer.nsecsElapsed();
    std::cout << mediumvec << " ";
    std::cout << "Needed " << duration << " nanoseconds for the calculation!" << std::endl;

    timer.restart();
    CCoordinateGeodetic endVec = CCoordinateTransformation::toGeodetic(mediumvec);
    duration = timer.nsecsElapsed();

    std::cout << endVec << " ";
    std::cout << "Needed " << duration << " nanoseconds for the calculation!" << std::endl;

    std::cout << "-----------------------------------------------" << std::endl;
    return 0;
}

} // namespace
