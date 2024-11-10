// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TEST_TESTSERVICEINTERFACE_H
#define SWIFT_MISC_TEST_TESTSERVICEINTERFACE_H

#include "misc/swiftmiscexport.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/flightplan.h"
#include "misc/aviation/track.h"
#include "misc/aviation/transponder.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/client.h"
#include "misc/network/clientlist.h"
#include "misc/network/user.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/propertyindexref.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/variantlist.h"

#include <QDBusAbstractInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QLatin1String>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QTextStream>

class QDBusConnection;
class QDBusObjectPath;
class QDBusVariant;

namespace swift::misc
{
    class CVariant;
    namespace simulation::FsCommon
    {
        class CAircraftCfgEntriesList;
    }
}

namespace swift::misc::test
{
    //! Proxy class for swift::misc::Test::CTestService. This part is the caller.
    class SWIFT_MISC_EXPORT ITestServiceInterface : public QDBusAbstractInterface
    {
        Q_OBJECT

    public:
        //! Constructor
        ITestServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

        //! Destructor
        virtual ~ITestServiceInterface();

        //! Ping via DBus, tests object marshalling/unmarshalling
        static int pingTests(ITestServiceInterface &testServiceInterface, bool verbose);

        //! @{
        //! DBus calls
        QDBusPendingReply<swift::misc::aviation::CAtcStation> getAtcStation()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("getAtcStation"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::fscommon::CAircraftCfgEntriesList> getAircraftCfgEntriesList(int number)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(number);
            return asyncCallWithArgumentList(QLatin1String("getAircraftCfgEntriesList"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAtcStationList> getAtcStationList(int number)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(number);
            return asyncCallWithArgumentList(QLatin1String("getAtcStationList"), argumentList);
        }

        QDBusPendingReply<QList<QDBusObjectPath>> getObjectPaths(int number)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(number);
            return asyncCallWithArgumentList(QLatin1String("getObjectPaths"), argumentList);
        }

        QDBusPendingReply<swift::misc::physical_quantities::CSpeed> getSpeed()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("getSpeed"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAltitude> pingAltitude(const swift::misc::aviation::CAltitude &altitude)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(altitude);
            return asyncCallWithArgumentList(QLatin1String("pingAltitude"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAircraftSituation> pingSituation(const swift::misc::aviation::CAircraftSituation &situation)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(situation);
            return asyncCallWithArgumentList(QLatin1String("pingSituation"), argumentList);
        }

        QDBusPendingReply<swift::misc::network::CUser> pingUser(const swift::misc::network::CUser &user)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(user);
            return asyncCallWithArgumentList(QLatin1String("pingUser"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CTransponder> pingTransponder(const swift::misc::aviation::CTransponder &transponder)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(transponder);
            return asyncCallWithArgumentList(QLatin1String("pingTransponder"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAtcStation> pingAtcStation(const swift::misc::aviation::CAtcStation &station)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(station);
            return asyncCallWithArgumentList(QLatin1String("pingAtcStation"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAircraftIcaoCode> pingAircraftIcaoData(const swift::misc::aviation::CAircraftIcaoCode &icaoData)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(icaoData);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftIcaoData"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAircraftLights> pingAircraftLights(const swift::misc::aviation::CAircraftLights &lights)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(lights);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftLights"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAircraftParts> pingAircraftParts(const swift::misc::aviation::CAircraftParts &parts)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(parts);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftParts"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAircraftEngine> pingAircraftEngine(const swift::misc::aviation::CAircraftEngine &engine)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(engine);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftEngine"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::CAircraftModel> pingAircraftModel(const swift::misc::simulation::CAircraftModel &model)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(model);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftModel"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::CAircraftModelList> pingAircraftModelList(const swift::misc::simulation::CAircraftModelList &model)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(model);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftModelList"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::CSimulatedAircraft> pingSimulatedAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(aircraft);
            return asyncCallWithArgumentList(QLatin1String("pingSimulatedAircraft"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::CSimulatorPluginInfo> pingPluginInfo(const swift::misc::simulation::CSimulatorPluginInfo &info)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(info);
            return asyncCallWithArgumentList(QLatin1String("pingPluginInfo"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAtcStationList> pingAtcStationList(const swift::misc::aviation::CAtcStationList &atcStationList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(atcStationList);
            return asyncCallWithArgumentList(QLatin1String("pingAtcStationList"), argumentList);
        }

        QDBusPendingReply<swift::misc::physical_quantities::CSpeed> pingSpeed(const swift::misc::physical_quantities::CSpeed &speed)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(speed);
            return asyncCallWithArgumentList(QLatin1String("pingSpeed"), argumentList);
        }

        QDBusPendingReply<swift::misc::simulation::CSimulatedAircraftList> pingAircraftList(const swift::misc::simulation::CSimulatedAircraftList &aircraftList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(aircraftList);
            return asyncCallWithArgumentList(QLatin1String("pingAircraftList"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAirportList> pingAirportList(const swift::misc::aviation::CAirportList &airportList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(airportList);
            return asyncCallWithArgumentList(QLatin1String("pingAirportList"), argumentList);
        }

        QDBusPendingReply<swift::misc::network::CClientList> pingClientList(const swift::misc::network::CClientList &clientList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(clientList);
            return asyncCallWithArgumentList(QLatin1String("pingClientList"), argumentList);
        }

        QDBusPendingReply<swift::misc::network::CClient> pingClient(const swift::misc::network::CClient &client)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(client);
            return asyncCallWithArgumentList(QLatin1String("pingClient"), argumentList);
        }

        QDBusPendingReply<swift::misc::CVariant> pingCVariant(const swift::misc::CVariant &variant)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(variant);
            return asyncCallWithArgumentList(QLatin1String("pingCVariant"), argumentList);
        }

        QDBusPendingReply<swift::misc::CPropertyIndex> pingPropertyIndex(const swift::misc::CPropertyIndex &index)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(index);
            return asyncCallWithArgumentList(QLatin1String("pingPropertyIndex"), argumentList);
        }

        QDBusPendingReply<swift::misc::CPropertyIndexVariantMap> pingIndexVariantMap(const swift::misc::CPropertyIndexVariantMap &indexVariantMap)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(indexVariantMap);
            return asyncCallWithArgumentList(QLatin1String("pingIndexVariantMap"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CFlightPlan> pingFlightPlan(const aviation::CFlightPlan &flightPlan)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(flightPlan);
            return asyncCallWithArgumentList(QLatin1String("pingFlightPlan"), argumentList);
        }

        QDBusPendingReply<swift::misc::aviation::CAltitude> receiveAltitude(const swift::misc::aviation::CAltitude &altitude)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(altitude);
            return asyncCallWithArgumentList(QLatin1String("receiveAltitude"), argumentList);
        }

        QDBusPendingReply<> receiveAtcStation(const swift::misc::aviation::CAtcStation &station)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(station);
            return asyncCallWithArgumentList(QLatin1String("receiveAtcStation"), argumentList);
        }

        QDBusPendingReply<> receiveAtcStationList(const swift::misc::aviation::CAtcStationList &atcStationList)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(atcStationList);
            return asyncCallWithArgumentList(QLatin1String("receiveAtcStationList"), argumentList);
        }

        QDBusPendingReply<> receiveCallsign(const swift::misc::aviation::CCallsign &callsign)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(callsign);
            return asyncCallWithArgumentList(QLatin1String("receiveCallsign"), argumentList);
        }

        QDBusPendingReply<> receiveComUnit(const swift::misc::aviation::CComSystem &comUnit)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(comUnit);
            return asyncCallWithArgumentList(QLatin1String("receiveComUnit"), argumentList);
        }

        QDBusPendingReply<> receiveGeoPosition(const swift::misc::geo::CCoordinateGeodetic &geo)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(geo);
            return asyncCallWithArgumentList(QLatin1String("receiveGeoPosition"), argumentList);
        }

        QDBusPendingReply<> receiveLength(const swift::misc::physical_quantities::CLength &length)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(length);
            return asyncCallWithArgumentList(QLatin1String("receiveLength"), argumentList);
        }

        QDBusPendingReply<> receiveVariantList(const swift::misc::CVariantList &list)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(list);
            return asyncCallWithArgumentList(QLatin1String("receiveVariantList"), argumentList);
        }

        QDBusPendingReply<> receiveList(const QList<double> &list)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(list);
            return asyncCallWithArgumentList(QLatin1String("receiveList"), argumentList);
        }

        QDBusPendingReply<> receiveSpeed(const swift::misc::physical_quantities::CSpeed &speed)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(speed);
            return asyncCallWithArgumentList(QLatin1String("receiveSpeed"), argumentList);
        }

        QDBusPendingReply<> receiveStringMessage(const QString &message)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(message);
            return asyncCallWithArgumentList(QLatin1String("receiveStringMessage"), argumentList);
        }

        QDBusPendingReply<> receiveTrack(const swift::misc::aviation::CTrack &track)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(track);
            return asyncCallWithArgumentList(QLatin1String("receiveTrack"), argumentList);
        }

        QDBusPendingReply<> receiveTransponder(const swift::misc::aviation::CTransponder &transponder)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(transponder);
            return asyncCallWithArgumentList(QLatin1String("receiveTransponder"), argumentList);
        }

        QDBusPendingReply<> receiveValueMap(const swift::misc::CPropertyIndexVariantMap &valueMap)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(valueMap);
            return asyncCallWithArgumentList(QLatin1String("receiveValueMap"), argumentList);
        }

        QDBusPendingReply<> receiveVariant(const QDBusVariant &variant, int localMetyType)
        {
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(variant) << QVariant::fromValue(localMetyType);
            return asyncCallWithArgumentList(QLatin1String("receiveVariant"), argumentList);
        }
        //! @}

    signals:
        //! send message
        void sendStringMessage(const QString &message);

    private:
        //! Interface name
        static const char *InterfaceName();

        //! Compare objects and output info
        template <class ValueObject>
        static bool pingCompare(const ValueObject &in, const ValueObject &out, QTextStream &ts, bool verbose, int &errors)
        {
            const bool equal = (in == out && extraCompare(in, out));
            if (!equal)
            {
                errors++;
                if (verbose) { ts << "I: " << in.toQString() << Qt::endl
                                  << "O: " << out.toQString() << Qt::endl; }
            }
            return equal;
        }

        //! @{
        //! Extra comparison step for some types
        template <class ValueObject>
        static bool extraCompare(const ValueObject &, const ValueObject &)
        {
            return true;
        }

        static bool extraCompare(const swift::misc::aviation::CFlightPlan &in, const swift::misc::aviation::CFlightPlan &out)
        {
            // flight plan: check units are preserved
            if (in.getEnrouteTime().getUnit() != out.getEnrouteTime().getUnit()) { return false; }
            if (in.getFuelTime().getUnit() != out.getFuelTime().getUnit()) { return false; }
            if (in.getCruiseAltitude().getUnit() != out.getCruiseAltitude().getUnit()) { return false; }
            if (in.getCruiseTrueAirspeed().getUnit() != out.getCruiseTrueAirspeed().getUnit()) { return false; }
            return true;
        }
        //! @}

        //! Error info string
        static const QString &errorInfo(bool ok);
    };
} // ns
#endif
