#ifndef BLACKMISCTEST_TESTSERVICE_INTERFACE_H
#define BLACKMISCTEST_TESTSERVICE_INTERFACE_H

#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/blackmiscallvalueclasses.h"
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

namespace BlackMiscTest
{

    /*!
     * \brief Proxy class for interface blackmisctest.testservice
     */
    class TestServiceInterface: public QDBusAbstractInterface
    {
        Q_OBJECT

    public:
        static inline const char *staticInterfaceName() { return "blackmisctest.testservice"; }

    public:
        //! \brief Constructor
        TestServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

        //! \brief Destructor
        ~TestServiceInterface();

    public Q_SLOTS:

        //! \brief DBus calls
        //! @{
        inline QDBusPendingReply<BlackMisc::Aviation::CAtcStation> getAtcStation()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("getAtcStation"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> getAtcStationList(int number)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(number);
            return asyncCallWithArgumentList(QLatin1String("getAtcStationList"), argumentList);
        }

        inline QDBusPendingReply<QList<QDBusObjectPath> > getObjectPaths(int number)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(number);
            return asyncCallWithArgumentList(QLatin1String("getObjectPaths"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::PhysicalQuantities::CSpeed> getSpeed()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("getSpeed"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::Aviation::CAltitude> pingAltitude(BlackMisc::Aviation::CAltitude altitude)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(altitude);
            return asyncCallWithArgumentList(QLatin1String("pingAltitude"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::Aviation::CAtcStation> pingAtcStation(BlackMisc::Aviation::CAtcStation station)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(station);
            return asyncCallWithArgumentList(QLatin1String("pingAtcStation"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::Aviation::CAircraft> pingAircraft(BlackMisc::Aviation::CAircraft aircraft)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(aircraft);
            return asyncCallWithArgumentList(QLatin1String("pingAircraft"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> pingAtcStationList(BlackMisc::Aviation::CAtcStationList AtcStationList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(AtcStationList);
            return asyncCallWithArgumentList(QLatin1String("pingAtcStationList"), argumentList);
        }

        inline QDBusPendingReply<BlackMisc::PhysicalQuantities::CSpeed> pingSpeed(BlackMisc::PhysicalQuantities::CSpeed speed)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(speed);
            return asyncCallWithArgumentList(QLatin1String("pingSpeed"), argumentList);
        }

        inline QDBusPendingReply<> receiveAltitude(BlackMisc::Aviation::CAltitude altitude)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(altitude);
            return asyncCallWithArgumentList(QLatin1String("receiveAltitude"), argumentList);
        }

        inline QDBusPendingReply<> receiveAtcStation(BlackMisc::Aviation::CAtcStation station)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(station);
            return asyncCallWithArgumentList(QLatin1String("receiveAtcStation"), argumentList);
        }

        inline QDBusPendingReply<> receiveAtcStationList(BlackMisc::Aviation::CAtcStationList AtcStationList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(AtcStationList);
            return asyncCallWithArgumentList(QLatin1String("receiveAtcStationList"), argumentList);
        }

        inline QDBusPendingReply<> receiveCallsign(BlackMisc::Aviation::CCallsign callsign)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(callsign);
            return asyncCallWithArgumentList(QLatin1String("receiveCallsign"), argumentList);
        }

        inline QDBusPendingReply<> receiveComUnit(BlackMisc::Aviation::CComSystem comUnit)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(comUnit);
            return asyncCallWithArgumentList(QLatin1String("receiveComUnit"), argumentList);
        }

        inline QDBusPendingReply<> receiveGeoPosition(BlackMisc::Geo::CCoordinateGeodetic geo)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(geo);
            return asyncCallWithArgumentList(QLatin1String("receiveGeoPosition"), argumentList);
        }

        inline QDBusPendingReply<> receiveLength(BlackMisc::PhysicalQuantities::CLength length)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(length);
            return asyncCallWithArgumentList(QLatin1String("receiveLength"), argumentList);
        }

        inline QDBusPendingReply<> receiveLengthsQl(const QVariantList &lengthsList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(lengthsList);
            return asyncCallWithArgumentList(QLatin1String("receiveLengthsQl"), argumentList);
        }

        inline QDBusPendingReply<> receiveLengthsQvl(const QVariantList &lengthsVariantList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(lengthsVariantList);
            return asyncCallWithArgumentList(QLatin1String("receiveLengthsQvl"), argumentList);
        }

        inline QDBusPendingReply<> receiveList(const QList<double> &list)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(list);
            return asyncCallWithArgumentList(QLatin1String("receiveList"), argumentList);
        }

        inline QDBusPendingReply<> receiveMatrix(BlackMisc::Math::CMatrix3x3 matrix)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(matrix);
            return asyncCallWithArgumentList(QLatin1String("receiveMatrix"), argumentList);
        }

        inline QDBusPendingReply<> receiveSpeed(BlackMisc::PhysicalQuantities::CSpeed speed)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(speed);
            return asyncCallWithArgumentList(QLatin1String("receiveSpeed"), argumentList);
        }

        inline QDBusPendingReply<> receiveStringMessage(const QString &message)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(message);
            return asyncCallWithArgumentList(QLatin1String("receiveStringMessage"), argumentList);
        }

        inline QDBusPendingReply<> receiveTrack(BlackMisc::Aviation::CTrack track)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(track);
            return asyncCallWithArgumentList(QLatin1String("receiveTrack"), argumentList);
        }

        inline QDBusPendingReply<> receiveTransponder(BlackMisc::Aviation::CTransponder transponder)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(transponder);
            return asyncCallWithArgumentList(QLatin1String("receiveTransponder"), argumentList);
        }

        inline QDBusPendingReply<> receiveValueMap(BlackMisc::CIndexVariantMap valueMap)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(valueMap);
            return asyncCallWithArgumentList(QLatin1String("receiveValueMap"), argumentList);
        }

        inline QDBusPendingReply<> receiveVariant(const QDBusVariant &variant, int localMetyType)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(variant) << QVariant::fromValue(localMetyType);
            return asyncCallWithArgumentList(QLatin1String("receiveVariant"), argumentList);
        }
        //! @}


    Q_SIGNALS:
        //! \brief send message
        void sendStringMessage(const QString &message);
    };
} // namespace

#endif
