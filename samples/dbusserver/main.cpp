#include <QCoreApplication>
#include <QDebug>
#include "blackcore/dbus_server.h"
#include "aircraft_manager_handler.h"
#include "aircraft_manager.h"
#include "atc_manager_handler.h"
#include "atc_manager.h"
#include "fsd_client.h"
#include "fsd_client_handler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CRemoteAircraft::registerMetaType();

    BlackCore::CDBusServer server("tcp:host=127.0.0.1,port=45000");

    // Setting up our objects
    CAircraftManager aircraftManager;

    aircraftManager.addAircraft(CRemoteAircraft("DLH456"));
    aircraftManager.addAircraft(CRemoteAircraft("DLH555"));
    aircraftManager.addAircraft(CRemoteAircraft("DLH666"));

    CAtcManager atcManager;

    atcManager.addATC("EDDM_N_TWR");
    atcManager.addATC("KJFK_GND");
    atcManager.addATC("LOWW_CTR");

    CFsdClient fsdclient;


    // Setting up the handler to expose the objects via DBus
    CAircraftManagerHandler aircraftManagerHandler(&aircraftManager);
    CAtcManagerHandler atcManagerHandler(&atcManager);
    CFsdClientHandler fsdClientHandler (&fsdclient);

    // Pass the DBus server to the handlers. This registers also
    // the handler in the DBus server and makes it available
    // via the interface.
    aircraftManagerHandler.setDBusServer(&server);
    atcManagerHandler.setDBusServer(&server);
    fsdClientHandler.setDBusServer(&server);

    return a.exec();
}
