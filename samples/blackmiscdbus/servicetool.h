#ifndef BLACKMISCTEST_SERVICETOOL_H
#define BLACKMISCTEST_SERVICETOOL_H

#include "blackcore/dbus_server.h"
#include "blackmisc/avatcstationlist.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>

namespace BlackMiscTest
{
    class Testservice; // forward declaration

    /*!
     * \brief Supporting / helper functions for running the tests
     */
    class ServiceTool
    {
    private:
        ServiceTool() {}

    public:
        /*!
         * \brief Get process id
         * \return
         */
        static qint64 getPid()
        {
            return QCoreApplication::applicationPid();
        }

        /*!
         * \brief Client side of data transfer test
         * \param address
         */
        static void dataTransferTestClient(const QString &address);

        /*!
         * \brief Server side of data transfer test
         */
        static void dataTransferTestServer(BlackCore::CDBusServer *dBusServer);

        /*!
         * \brief Start a new process
         * \param executable
         * \param arguments
         * \param parent
         * \return
         */
        static QProcess *startNewProcess(const QString &executable, const QStringList &arguments = QStringList(), QObject *parent = 0);

        /*!
         * \brief Loop to send data to test service (slots on server)
         * \param connection
         */
        static void sendDataToTestservice(const QDBusConnection &connection);

        /*!
         * \brief Display QDBusArgument
         * \param arg
         */
        static void displayQDBusArgument(const QDBusArgument &arg, qint32 level = 0);

        /*!
         * \brief Register testservice with connection
         * \param connection
         * \param parent
         * \return test service object
         */
        static Testservice *registerTestservice(QDBusConnection &connection, QObject *parent = 0);

        /*!
         * \brief Get a random callsign
         * \return
         */
        static BlackMisc::Aviation::CCallsign getRandomCallsign();

        /*!
         * \brief Get stations
         * \param number
         * \return
         */
        static BlackMisc::Aviation::CAtcStationList getStations(qint32 number);

    };

} // namespace

#endif // guard
