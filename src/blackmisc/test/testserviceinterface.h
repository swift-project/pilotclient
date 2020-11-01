/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_TESTSERVICEINTERFACE_H
#define BLACKMISC_TEST_TESTSERVICEINTERFACE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/track.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/variantlist.h"

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

namespace BlackMisc
{
    class CVariant;
    namespace Simulation { namespace FsCommon { class CAircraftCfgEntriesList; } }
}

namespace BlackMisc
{
    namespace Test
    {
        //! Proxy class for BlackMisc::Test::CTestService. This part is the caller.
        class BLACKMISC_EXPORT ITestServiceInterface: public QDBusAbstractInterface
        {
            Q_OBJECT

        public:
            //! Constructor
            ITestServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

            //! Destructor
            virtual ~ITestServiceInterface();

            //! Ping via DBus, tests object marshalling/unmarshalling
            static int pingTests(ITestServiceInterface &testServiceInterface, bool verbose);

            //! DBus calls
            //! @{
            QDBusPendingReply<BlackMisc::Aviation::CAtcStation> getAtcStation()
            {
                QList<QVariant> argumentList;
                return asyncCallWithArgumentList(QLatin1String("getAtcStation"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList> getAircraftCfgEntriesList(int number)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(number);
                return asyncCallWithArgumentList(QLatin1String("getAircraftCfgEntriesList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> getAtcStationList(int number)
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

            QDBusPendingReply<BlackMisc::PhysicalQuantities::CSpeed> getSpeed()
            {
                QList<QVariant> argumentList;
                return asyncCallWithArgumentList(QLatin1String("getSpeed"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAltitude> pingAltitude(const BlackMisc::Aviation::CAltitude &altitude)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(altitude);
                return asyncCallWithArgumentList(QLatin1String("pingAltitude"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAircraftSituation> pingSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(situation);
                return asyncCallWithArgumentList(QLatin1String("pingSituation"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Network::CUser> pingUser(const BlackMisc::Network::CUser &user)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(user);
                return asyncCallWithArgumentList(QLatin1String("pingUser"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CTransponder> pingTransponder(const BlackMisc::Aviation::CTransponder &transponder)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(transponder);
                return asyncCallWithArgumentList(QLatin1String("pingTransponder"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAtcStation> pingAtcStation(const BlackMisc::Aviation::CAtcStation &station)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(station);
                return asyncCallWithArgumentList(QLatin1String("pingAtcStation"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAircraftIcaoCode> pingAircraftIcaoData(const BlackMisc::Aviation::CAircraftIcaoCode &icaoData)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(icaoData);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftIcaoData"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAircraftLights> pingAircraftLights(const BlackMisc::Aviation::CAircraftLights &lights)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(lights);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftLights"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAircraftParts> pingAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(parts);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftParts"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAircraftEngine> pingAircraftEngine(const BlackMisc::Aviation::CAircraftEngine &engine)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(engine);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftEngine"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::CAircraftModel> pingAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(model);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftModel"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::CAircraftModelList> pingAircraftModelList(const BlackMisc::Simulation::CAircraftModelList &model)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(model);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftModelList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::CSimulatedAircraft> pingSimulatedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(aircraft);
                return asyncCallWithArgumentList(QLatin1String("pingSimulatedAircraft"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::CSimulatorPluginInfo> pingPluginInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &info)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(info);
                return asyncCallWithArgumentList(QLatin1String("pingPluginInfo"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> pingAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(atcStationList);
                return asyncCallWithArgumentList(QLatin1String("pingAtcStationList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::PhysicalQuantities::CSpeed> pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(speed);
                return asyncCallWithArgumentList(QLatin1String("pingSpeed"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Simulation::CSimulatedAircraftList> pingAircraftList(const BlackMisc::Simulation::CSimulatedAircraftList &aircraftList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(aircraftList);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAirportList> pingAirportList(const BlackMisc::Aviation::CAirportList &airportList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(airportList);
                return asyncCallWithArgumentList(QLatin1String("pingAirportList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Network::CClientList> pingClientList(const BlackMisc::Network::CClientList &clientList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(clientList);
                return asyncCallWithArgumentList(QLatin1String("pingClientList"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Network::CClient> pingClient(const BlackMisc::Network::CClient &client)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(client);
                return asyncCallWithArgumentList(QLatin1String("pingClient"), argumentList);
            }

            QDBusPendingReply<BlackMisc::CVariant> pingCVariant(const BlackMisc::CVariant &variant)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(variant);
                return asyncCallWithArgumentList(QLatin1String("pingCVariant"), argumentList);
            }

            QDBusPendingReply<BlackMisc::CPropertyIndex> pingPropertyIndex(const BlackMisc::CPropertyIndex &index)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(index);
                return asyncCallWithArgumentList(QLatin1String("pingPropertyIndex"), argumentList);
            }

            QDBusPendingReply<BlackMisc::CPropertyIndexVariantMap> pingIndexVariantMap(const BlackMisc::CPropertyIndexVariantMap &indexVariantMap)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(indexVariantMap);
                return asyncCallWithArgumentList(QLatin1String("pingIndexVariantMap"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CFlightPlan> pingFlightPlan(const Aviation::CFlightPlan &flightPlan)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(flightPlan);
                return asyncCallWithArgumentList(QLatin1String("pingFlightPlan"), argumentList);
            }

            QDBusPendingReply<BlackMisc::Aviation::CAltitude> receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(altitude);
                return asyncCallWithArgumentList(QLatin1String("receiveAltitude"), argumentList);
            }

            QDBusPendingReply<> receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(station);
                return asyncCallWithArgumentList(QLatin1String("receiveAtcStation"), argumentList);
            }

            QDBusPendingReply<> receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(atcStationList);
                return asyncCallWithArgumentList(QLatin1String("receiveAtcStationList"), argumentList);
            }

            QDBusPendingReply<> receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(callsign);
                return asyncCallWithArgumentList(QLatin1String("receiveCallsign"), argumentList);
            }

            QDBusPendingReply<> receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(comUnit);
                return asyncCallWithArgumentList(QLatin1String("receiveComUnit"), argumentList);
            }

            QDBusPendingReply<> receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(geo);
                return asyncCallWithArgumentList(QLatin1String("receiveGeoPosition"), argumentList);
            }

            QDBusPendingReply<> receiveLength(const BlackMisc::PhysicalQuantities::CLength &length)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(length);
                return asyncCallWithArgumentList(QLatin1String("receiveLength"), argumentList);
            }

            QDBusPendingReply<> receiveVariantList(const BlackMisc::CVariantList &list)
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

            QDBusPendingReply<> receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
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

            QDBusPendingReply<> receiveTrack(const BlackMisc::Aviation::CTrack &track)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(track);
                return asyncCallWithArgumentList(QLatin1String("receiveTrack"), argumentList);
            }

            QDBusPendingReply<> receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(transponder);
                return asyncCallWithArgumentList(QLatin1String("receiveTransponder"), argumentList);
            }

            QDBusPendingReply<> receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap)
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
            template<class ValueObject>
            static bool pingCompare(const ValueObject &in, const ValueObject &out, QTextStream &ts, bool verbose, int &errors)
            {
                const bool equal = (in == out && extraCompare(in, out));
                if (!equal)
                {
                    errors++;
                    if (verbose) { ts << "I: " << in.toQString() << Qt::endl << "O: " << out.toQString() << Qt::endl; }
                }
                return equal;
            }

            //! Extra comparison step for some types
            //! @{
            template<class ValueObject>
            static bool extraCompare(const ValueObject &, const ValueObject &) { return true; }

            static bool extraCompare(const BlackMisc::Aviation::CFlightPlan &in, const BlackMisc::Aviation::CFlightPlan &out)
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
} // ns
#endif
