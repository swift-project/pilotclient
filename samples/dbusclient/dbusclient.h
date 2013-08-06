#ifndef DBUSCLIENT_H
#define DBUSCLIENT_H

#include <QObject>
#include <QDBusConnection>
#include <QtDBus/QDBusError>

#include "aircraftmanager_interface.h"
#include "atcmanager_interface.h"
#include "fsdclient_interface.h"

class DBusClient : public QObject
{
    Q_OBJECT

private:
    QDBusConnection m_connection;
    org::vatsim::pilotClient::AircraftManager *aircraftManagerIface;
    org::vatsim::pilotClient::AtcManager *atcManagerIface;
    org::vatsim::pilotClient::FsdClient *fsdClientIface;

public:
    explicit DBusClient(QObject *parent = 0);
    ~DBusClient();

    void connectTo(const QString &host);
    void disconnectFrom();

    void printPilotList();
    void printAtcList();

    void printError();
    
signals:
    
public slots:

private slots:
    void slot_connected(const QString &host);
    void slot_disconnected();
    
};

#endif // DBUSCLIENT_H
