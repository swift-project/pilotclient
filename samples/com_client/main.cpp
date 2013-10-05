#include <QCoreApplication>

#include "blackmisc/context.h"
#include "client.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackMisc::CApplicationContext ctx;
    BlackMisc::IContext::setInstance(ctx);
    BlackMisc::CDebug debug;
    ctx.setObject(debug);

    Client client;
    
    return a.exec();
}
