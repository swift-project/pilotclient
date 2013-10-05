#include <QCoreApplication>
#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <limits>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackMisc::CApplicationContext ctx;
    BlackMisc::IContext::setInstance(ctx);
    BlackMisc::CDebug debug;
    ctx.setObject(debug);

    bAppInfo << "This is a Info log message";
    bAppWarning << "This is a bWarning log message";
    bAppError << "This is a bError log message";
    bAppDebug << "This is a bDebug log message";

    return a.exec();
}
