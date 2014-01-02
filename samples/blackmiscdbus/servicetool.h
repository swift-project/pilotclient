#ifndef BLACKMISCTEST_SERVICETOOL_H
#define BLACKMISCTEST_SERVICETOOL_H

#include "blackcore/dbus_server.h"
#include "datacontext.h"
#include "dummysignalslot.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>

class DatacontextAdaptor;

namespace BlackMiscTest
{

    class Testservice; // forward declaration

    /*!
     * \brief Supporting functions for running the tests
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
         * \brief Context test, server side
         * \param dBusServer
         */
        static void contextTestServer(BlackCore::CDBusServer *dBusServer);

        /*!
         * \brief Context test, client side
         * \param address
         */
        static void contextTestClient(const QString &address);

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
         * \brief Server loop
         * \param server
         * \param dataContext
         * \param dataContextAdaptor
         * \param dummySignalSlot
         */
        static void serverLoop(BlackCore::CDBusServer *server, CDataContext *dataContext, DatacontextAdaptor *dataContextAdaptor, CDummySignalSlot *dummySignalSlot);

        /*!
         * \brief Client loop
         * \param connection
         */
        static void clientLoop(QDBusConnection &connection, CDummySignalSlot *dummyObjectSignalSlot);

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
