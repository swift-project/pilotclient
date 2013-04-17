#include <QCoreApplication>
#include <QElapsedTimer>

#include <iostream>

#include "blackcore/matrix_3d.h"
#include "blackcore/vector_geo.h"
#include "blackcore/vector_3d.h"
#include "blackcore/interpolator.h"
#include "blackmisc/context.h"
#include "blackmisc/debug.h"

using namespace std;
using namespace BlackCore;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackMisc::IContext::getInstance().setSingleton(new BlackMisc::CDebug());

    QElapsedTimer timer;

    CVectorGeo myGeo(48.123, 11.75, 400);
    CVector3D vecNed(1, 0, 0);
    CVector3D vecEcef;

    CInterpolator interpolator;

    interpolator.initialize();

    CVectorGeo vecGeo(48.340733, 11.750565, 100);
    CVectorGeo vecGeo2(48.344727, 11.805153, 100);

    cout << "Start position: "  << endl;
	vecGeo.print();

    cout << "End position: "    << endl;
	vecGeo2.print();

    timer.start();

    // CVectorGeo pos, double groundVelocity, double heading, double pitch, double bank
    interpolator.pushUpdate(vecGeo, 20, 80, 0, 0);
    interpolator.pushUpdate(vecGeo2, 20, 250, 0, 0);

    double duration = timer.nsecsElapsed();

    TPlaneState teststate;
	
	timer.restart();
    interpolator.stateNow(&teststate);

    CEcef pos = teststate.position;
	CVector3D vel = teststate.velocity;
	CNed ned = teststate.velNED;

    duration = timer.nsecsElapsed();
	timer.restart();

	CVectorGeo resultGeo = pos.toGeodetic();
    duration = timer.nsecsElapsed();

    cout << "End position: "    << endl;
	resultGeo.print();
    cout << endl;
	
	cout << "End velocity: "    << endl;
	vel.print();
    cout << endl;

	cout << "Heading: "    << endl;
	cout << teststate.orientation.heading << endl;
    cout << endl;

	cout << duration <<  " nanoseconds" << endl;

    return a.exec();
}
