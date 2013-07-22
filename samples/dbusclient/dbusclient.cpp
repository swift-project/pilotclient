#include <QDebug>
#include <QStringList>

#include "dbusclient.h"


DBusClient::DBusClient(QObject *parent) :
    QObject(parent), m_connection("daemon")
{
    m_connection = QDBusConnection::connectToPeer("tcp:host=127.0.0.1,port=6668", "daemon");

    if(!m_connection.isConnected())
    {
        qWarning() << m_connection.lastError().message();
    }

    pilotManagerIface = new org::vatsim::pilotClient::PlaneManager("org.vatsim.PilotClient", "/PlaneManager", m_connection, this);
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
    //iface->disconnectFrom();
}

void DBusClient::slot_connected( const QString & host)
{
    qDebug() << "Conntected to " << host;
    qDebug() << "So lets collect some information... ";


    printPilotList();
    printAtcList();

    //qDebug() << iface->getMetar("EDDM");
}

void DBusClient::slot_disconnected()
{
    qDebug() << "Disconntected from Vatsim";
}

void DBusClient::printPilotList()
{
    Q_ASSERT(pilotManagerIface);

    qDebug() << "Online pilots: ";

    QStringList planes = pilotManagerIface->pilotList();
    if (planes.isEmpty())
    {
        qWarning() << "Got no pilots. Something went wrong!";
        return;
    }

    foreach(QString plane, planes)
    {
        qDebug() << plane;
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
