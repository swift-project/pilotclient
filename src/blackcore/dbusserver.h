#ifndef BLACKCORE_DBUSSERVER_H
#define BLACKCORE_DBUSSERVER_H

#include <QObject>
#include <QtDBus/QDBusServer>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusConnection>
#include <QStringList>
#include <QMap>

namespace BlackCore
{

    class CDBusServer : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.vatsim.pilotClient")

    private:
        QDBusServer m_busServer; //!< QDBusServer implementation

        QMap<QString, QObject*> m_objects; //!< Mapping of all exposed objects

        QMap<QString, QDBusConnection> m_DBusConnections; //!< Mapping of all DBusConnection objects

    public:

        /*!
         * \brief Constructor
         * \param parent
         */
        CDBusServer(QObject *parent = 0);

        /*!
         * \brief Adds a QObject to be exposed to DBus
         * \param name
         * \param object
         */
        void addObject(const QString &name, QObject *object);


        void printError();

    public slots:
        void newConnection(const QDBusConnection & connection);

    };

} // namespace BlackCore

#endif // BLACKCORE_DBUSSERVER_H
