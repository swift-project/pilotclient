#include <QDebug>
#include <QStringList>

#include "remote_aircraft.h"

#include "dbusclient.h"


DBusClient::DBusClient(QObject *parent) :
    QObject(parent), m_connection("daemon")
{
    m_connection = QDBusConnection::connectToPeer("tcp:host=127.0.0.1,port=45000", "daemon");

    if(!m_connection.isConnected())
    {
        qWarning() << m_connection.lastError().message();
    }

    aircraftManagerIface = new org::vatsim::pilotClient::AircraftManager("org.vatsim.PilotClient", "/AircraftManager", m_connection, this);
    atcManagerIface = new org::vatsim::pilotClient::AtcManager("org.vatsim.PilotClien", "/AtcManager", m_connection, this);
    fsdClientIface = new org::vatsim::pilotClient::FsdClient("org.vatsim.PilotClient", "/FsdClient", m_connection, this);
    connect(fsdClientIface, &org::vatsim::pilotClient::FsdClient::connectedTo, this, &DBusClient::slot_connected);
}

DBusClient::~DBusClient()
{

}

void DBusClient::connectTo(const QString &host)
{
    fsdClientIface->connectTo(host);
}

void DBusClient::disconnectFrom()
{
}

void DBusClient::slot_connected( const QString & host)
{
    qDebug() << "Conntected to " << host;
    qDebug() << "Retrieve all visible aircrafts and controllers...";


    printPilotList();
    printAtcList();
}

void DBusClient::slot_disconnected()
{
    qDebug() << "Disconntected from Vatsim";
}

void DBusClient::printPilotList()
{
    Q_ASSERT(aircraftManagerIface);

    qDebug() << "Online aircrafts: ";

    CRemoteAircraftList aircrafts = aircraftManagerIface->aircraftList();
    if (aircrafts.isEmpty())
    {
        qWarning() << "Got no aircrafts. Something went wrong!";
        return;
    }
    foreach(CRemoteAircraft aircraft, aircrafts)
    {
        qDebug() << aircraft.getCallsign();
    }
}

void DBusClient::printAtcList()
{
    Q_ASSERT(atcManagerIface);

    qDebug() << "Online controllers: ";

    QStringList controllers = atcManagerIface->atcList();

    if (controllers.isEmpty())
    {
        qWarning() << "Got no controllers. Something went wrong!";
        return;
    }

    foreach(QString controller, controllers)
    {
        qDebug() << controller;
    }
}

void DBusClient::printError()
{
    qWarning() << m_connection.lastError().message();
}
