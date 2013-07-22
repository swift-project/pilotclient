#ifndef FSDCLIENTHANDLER_H
#define FSDCLIENTHANDLER_H

#include <QObject>


namespace BlackCore
{
    class CDBusServer;
}

class CFsdClient;

class CFsdClientHandler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.FsdClient")

    BlackCore::CDBusServer *m_dbusserver;
    CFsdClient *m_parent;

    QString m_dbusPath;

public:
    CFsdClientHandler(QObject *parent = 0);
    ~CFsdClientHandler() {}

    Q_INVOKABLE void connectTo(const QString &host);

    void setDBusServer(BlackCore::CDBusServer *dbusServer);
    
signals:
    void connectedTo( const QString &host);
    
public slots:
    
};

#endif // FSDCLIENTHANDLER_H
