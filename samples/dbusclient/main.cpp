#include <QCoreApplication>
#include "dbusclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DBusClient client;

    client.connectTo("dev.vatsim-germany.org");
    
    return a.exec();
}
