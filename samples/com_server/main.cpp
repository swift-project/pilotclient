#include <QCoreApplication>

#include "blackmisc/context.h"
#include "server.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    BlackMisc::CApplicationContext myApplicationContext;
    QCoreApplication a(argc, argv);

	Server server;
    
    return a.exec();
}
