#include <QCoreApplication>
#include <QDebug>
#include "dbusclient.h"
#include "remote_aircraft.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "***********************************************";
    qDebug() << "******* Welcome to DBus Client sample *********";
    qDebug() << "* This samples demonstrates the communication *";
    qDebug() << "* between two different processes, simulating *";
    qDebug() << "* a client connection to DBusSever by P2P and *";
    qDebug() << "* and asking for online aircrafts and ATC     *";
    qDebug() << "* controllers. Use DBusServer sample to get   *";
    qDebug() << "* aircraft and contoller objects from the     *";
    qDebug() << "* FSD network They can be accessed by the     *";
    qDebug() << "* client after they have been transfered. Be  *";
    qDebug() << "* aware: All objects are simulated and not    *";
    qDebug() << "* VATSIM realtime traffic.                    *";
    qDebug() << "***********************************************";
    qDebug();

    DBusClient client;

    CRemoteAircraft::registerMetaType();

    client.connectTo("dev.vatsim-germany.org");
    
    return a.exec();
}
