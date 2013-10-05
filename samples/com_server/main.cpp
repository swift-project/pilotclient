#include <QCoreApplication>

#include "blackmisc/context.h"
#include "server.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackMisc::CApplicationContext ctx;
    BlackMisc::IContext::setInstance(ctx);
    BlackMisc::CDebug debug;
    ctx.setObject(debug);

    Server server;
    
    return a.exec();
}
