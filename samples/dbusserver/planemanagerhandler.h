#ifndef PLANEMANAGERHANDLER_H
#define PLANEMANAGERHANDLER_H

#include <QObject>
#include "planemanager.h"
#include "blackcore/dbusserver.h"

class CPlaneManagerHandler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.PlaneManager")
    Q_PROPERTY( QStringList pilotList READ pilotList)

public:
    CPlaneManagerHandler(QObject *parent = NULL);
    ~CPlaneManagerHandler() {}

    void setDBusServer(BlackCore::CDBusServer *dbusServer);
    
signals:
    
public slots:

private:
    QStringList pilotList() const;

    BlackCore::CDBusServer *m_dbusserver;
    CPlaneManager *m_parent;

    QString m_dbusPath;
    
};

#endif // PLANEMANAGERHANDLER_H
