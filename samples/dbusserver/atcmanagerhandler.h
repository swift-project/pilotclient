#ifndef ATCMANAGERHANDLER_H
#define ATCMANAGERHANDLER_H

#include <QObject>
#include <QStringList>

namespace BlackCore
{
    class CDBusServer;
}

class CAtcManager;

class CAtcManagerHandler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient.AtcManager")
    Q_PROPERTY( QStringList atcList READ atcList)

    QStringList atcList() const;

    BlackCore::CDBusServer *m_dbusserver;
    CAtcManager *m_parent;

    QString m_dbusPath;

public:
    CAtcManagerHandler(QObject *parent = NULL);
    ~CAtcManagerHandler();

    void setDBusServer(BlackCore::CDBusServer *dbusServer);


};

#endif // ATCMANAGERHANDLER_H
