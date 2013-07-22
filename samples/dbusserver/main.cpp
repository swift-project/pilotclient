#include <QCoreApplication>
#include <QDebug>
#include "blackcore/dbusserver.h"
#include "planemanagerhandler.h"
#include "atcmanagerhandler.h"
#include "atcmanager.h"
#include "fsdclient.h"
#include "fsdclienthandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackCore::CDBusServer server;

    // Setting up our objects
    CPlaneManager planeManager;
    CAtcManager atcManager;
    CFsdClient fsdclient;


    // Setting up the handler to expose the objects via DBus
    CPlaneManagerHandler planeManagerHandler(&planeManager);
    CAtcManagerHandler atcManagerHandler(&atcManager);
    CFsdClientHandler fsdClientHandler (&fsdclient);

    // Pass the DBus server to the handlers. This registers also
    // the handler in the DBus server and makes it available
    // via the interface.
    planeManagerHandler.setDBusServer(&server);
    atcManagerHandler.setDBusServer(&server);
    fsdClientHandler.setDBusServer(&server);

    return a.exec();
}
