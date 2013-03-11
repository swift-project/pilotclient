#include <QCoreApplication>
#include <blackmisc/debug.h>
#include <blackcore/constants.h>
#include <limits>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMisc::CApplicationContext myApplicationContext;

    bInfo << "This is a Info log message";
    bWarning << "This is a bWarning log message";
    bError << "This is a bError log message";
    bDebug << "This is a bDebug log message";
    
    return a.exec();
}
