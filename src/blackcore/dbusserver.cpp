#include <QDebug>

#include "dbusserver.h"

namespace BlackCore
{

// TODO:
// - Change constructor to use address from the config file
// - Ammend debug message according to the final result in the forum discussion

    CDBusServer::CDBusServer(QObject *parent) :
        QObject(parent), m_busServer("tcp:host=127.0.0.1,port=6668", parent)
    {
        if (!m_busServer.isConnected())
        {
            qWarning() << m_busServer.lastError().message();
        }
        else
        {
            qDebug() << "Server is running on" << m_busServer.address();
        }

        connect(&m_busServer, &QDBusServer::newConnection, this, &CDBusServer::newConnection);
    }

    void CDBusServer::newConnection(const QDBusConnection & connection)
    {
        QMap<QString, QObject*>::ConstIterator i = m_objects.begin();
        QDBusConnection newConnection(connection);

        m_DBusConnections.insert(newConnection.name(), newConnection);

        qDebug() << "New Connection from: " << newConnection.name();

        while (i != m_objects.end())
        {
            qDebug() << "Adding " << i.key() << "to the new connection.";
            newConnection.registerObject(i.key(), i.value());
            ++i;
        }
    }

    void CDBusServer::addObject(const QString &name, QObject *object)
    {
        m_objects.insert(name, object);
    }

    void CDBusServer::printError()
    {
        qWarning() << m_busServer.lastError().name();
    }

} // namespace BlackCore


