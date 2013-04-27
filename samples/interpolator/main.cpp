/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/coordinatetransformation.h"
#include "blackmisc/pqangle.h"
#include "blackcore/interpolator.h"
#include "blackmisc/context.h"
#include "blackmisc/debug.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <iostream>

using namespace std;
using namespace BlackCore;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMisc::IContext::getInstance().setSingleton(new BlackMisc::CDebug());

    QElapsedTimer timer;

//    CCoordinateGeodetic myGeo(48.123, 11.75, 400);
//    CVector3D vecNed(1, 0, 0);
//    CVector3D vecEcef;

    CInterpolator interpolator;
    interpolator.initialize();

    CCoordinateGeodetic vecGeo(48.340733, 11.750565, 100);
    CCoordinateGeodetic vecGeo2(48.344727, 11.805153, 100);

    cout << "Start position: "  << vecGeo << endl;
    cout << "End position: "    << vecGeo2 << endl;

    timer.start();

    // CVectorGeo pos, double groundVelocity, double heading, double pitch, double bank
    CAngle zeroAngle(0, CAngleUnit::deg());
    CSpeed speed(20, CSpeedUnit::kts());
    CCoordinateNed ned;

    ned = interpolator.pushUpdate(vecGeo, speed, CHeading(80, false, CAngleUnit::deg()), zeroAngle, zeroAngle);
    cout << "Interpolator NED 1: "  << ned << endl;
    ned = interpolator.pushUpdate(vecGeo2, speed, CHeading(250, false, CAngleUnit::deg()), zeroAngle, zeroAngle);
    cout << "Interpolator NED 2: "  << ned << endl;

    double duration = timer.nsecsElapsed();
    TPlaneState teststate;
    timer.restart();
    interpolator.stateNow(&teststate);

    CCoordinateEcef pos = teststate.position;
    CVector3D vel = teststate.velocity;
    ned = teststate.velNED;

    duration = timer.nsecsElapsed();

    CCoordinateTransformation::toGeodetic(pos);
    timer.restart();

    CCoordinateGeodetic resultGeo = CCoordinateTransformation::toGeodetic(pos);
    duration = timer.nsecsElapsed();

    cout << "End position: "  << resultGeo << endl;
    cout << "End velocity: "  << vel << endl;
    cout << "End "  << ned << endl;
    cout << "Heading: ";
    cout << teststate.orientation.heading.switchUnit(CAngleUnit::deg()) << endl;
    cout << endl;
    cout << duration <<  " nanoseconds" << endl;

    return a.exec();
}
