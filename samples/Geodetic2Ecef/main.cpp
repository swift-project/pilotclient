#include <QCoreApplication>
#include <QElapsedTimer>
#include "blackmisc/debug.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

#include "blackcore/ecef.h"
#include "blackcore/vector_geo.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMisc::CApplicationContext myApplicationContext;
	
	QElapsedTimer timer;
	qint64 duration;

    double lat = 27.999999, lon = 86.999999, h = 8820.999999; // Mt Everest
	
	BlackCore::CEcef mediumvec;
	BlackCore::CVectorGeo startVec(lat, lon, h);
	startVec.print();
	
	cout << std::endl;
	cout << std::endl;
	
	BlackCore::CVectorGeo endVec;

	timer.start();
	mediumvec = startVec.toCartesian();
	
	duration = timer.nsecsElapsed();
	
	mediumvec.print();
	
	cout << std::endl;
	cout << std::endl;
	
	cout << "Needed " << duration << " nanoseconds for the calculation!" << std::endl << std::endl;
	
	timer.restart();
	endVec = mediumvec.toGeodetic();
	duration = timer.nsecsElapsed();
	
	endVec.print();
	
	cout << "Needed " << duration << " nanoseconds for the calculation!" << std::endl << std::endl;
    
    return a.exec();
}
