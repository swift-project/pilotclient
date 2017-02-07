/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_TEST_TESTSERVICEINTERFACE_H
#define BLACKCORE_TEST_TESTSERVICEINTERFACE_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/track.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindex.h"
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

namespace BlackCore
{
    namespace Test
    {
        //! Proxy class for BlackCore::Test::TestService. This part is the caller.
        class BLACKCORE_EXPORT ITestServiceInterface: public QDBusAbstractInterface
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
            inline QDBusPendingReply<BlackMisc::Aviation::CAtcStation> getAtcStation()
            {
                QList<QVariant> argumentList;
                return asyncCallWithArgumentList(QLatin1String("getAtcStation"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList> getAircraftCfgEntriesList(int number)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(number);
                return asyncCallWithArgumentList(QLatin1String("getAircraftCfgEntriesList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> getAtcStationList(int number)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(number);
                return asyncCallWithArgumentList(QLatin1String("getAtcStationList"), argumentList);
            }

            inline QDBusPendingReply<QList<QDBusObjectPath>> getObjectPaths(int number)
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

            inline QDBusPendingReply<BlackMisc::Aviation::CAltitude> pingAltitude(const BlackMisc::Aviation::CAltitude &altitude)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(altitude);
                return asyncCallWithArgumentList(QLatin1String("pingAltitude"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAircraftSituation> pingSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(situation);
                return asyncCallWithArgumentList(QLatin1String("pingSituation"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Network::CUser> pingUser(const BlackMisc::Network::CUser &user)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(user);
                return asyncCallWithArgumentList(QLatin1String("pingUser"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CTransponder> pingTransponder(const BlackMisc::Aviation::CTransponder &transponder)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(transponder);
                return asyncCallWithArgumentList(QLatin1String("pingTransponder"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAtcStation> pingAtcStation(const BlackMisc::Aviation::CAtcStation &station)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(station);
                return asyncCallWithArgumentList(QLatin1String("pingAtcStation"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAircraftIcaoCode> pingAircraftIcaoData(const BlackMisc::Aviation::CAircraftIcaoCode &icaoData)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(icaoData);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftIcaoData"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAircraftLights> pingAircraftLights(const BlackMisc::Aviation::CAircraftLights &lights)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(lights);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftLights"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAircraftParts> pingAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(parts);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftParts"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAircraftEngine> pingAircraftEngine(const BlackMisc::Aviation::CAircraftEngine &engine)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(engine);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftEngine"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::CAircraftModel> pingAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(model);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftModel"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::CAircraftModelList> pingAircraftModelList(const BlackMisc::Simulation::CAircraftModelList &model)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(model);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftModelList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::CSimulatedAircraft> pingSimulatedAircraft(BlackMisc::Simulation::CSimulatedAircraft aircraft)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(aircraft);
                return asyncCallWithArgumentList(QLatin1String("pingSimulatedAircraft"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::CSimulatorPluginInfo> pingPluginInfo(BlackMisc::Simulation::CSimulatorPluginInfo info)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(info);
                return asyncCallWithArgumentList(QLatin1String("pingPluginInfo"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAtcStationList> pingAtcStationList(BlackMisc::Aviation::CAtcStationList atcStationList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(atcStationList);
                return asyncCallWithArgumentList(QLatin1String("pingAtcStationList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::PhysicalQuantities::CSpeed> pingSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(speed);
                return asyncCallWithArgumentList(QLatin1String("pingSpeed"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Simulation::CSimulatedAircraftList> pingAircraftList(const BlackMisc::Simulation::CSimulatedAircraftList &aircraftList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(aircraftList);
                return asyncCallWithArgumentList(QLatin1String("pingAircraftList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAirportList> pingAirportList(const BlackMisc::Aviation::CAirportList &airportList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(airportList);
                return asyncCallWithArgumentList(QLatin1String("pingAirportList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Network::CClientList> pingClientList(const BlackMisc::Network::CClientList &clientList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(clientList);
                return asyncCallWithArgumentList(QLatin1String("pingClientList"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Network::CClient> pingClient(const BlackMisc::Network::CClient &client)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(client);
                return asyncCallWithArgumentList(QLatin1String("pingClient"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::CVariant> pingCVariant(const BlackMisc::CVariant &variant)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(variant);
                return asyncCallWithArgumentList(QLatin1String("pingCVariant"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::CPropertyIndex> pingPropertyIndex(const BlackMisc::CPropertyIndex &index)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(index);
                return asyncCallWithArgumentList(QLatin1String("pingPropertyIndex"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::CPropertyIndexVariantMap> pingIndexVariantMap(BlackMisc::CPropertyIndexVariantMap indexVariantMap)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(indexVariantMap);
                return asyncCallWithArgumentList(QLatin1String("pingIndexVariantMap"), argumentList);
            }

            inline QDBusPendingReply<BlackMisc::Aviation::CAltitude> receiveAltitude(const BlackMisc::Aviation::CAltitude &altitude)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(altitude);
                return asyncCallWithArgumentList(QLatin1String("receiveAltitude"), argumentList);
            }

            inline QDBusPendingReply<> receiveAtcStation(const BlackMisc::Aviation::CAtcStation &station)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(station);
                return asyncCallWithArgumentList(QLatin1String("receiveAtcStation"), argumentList);
            }

            inline QDBusPendingReply<> receiveAtcStationList(const BlackMisc::Aviation::CAtcStationList &atcStationList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(atcStationList);
                return asyncCallWithArgumentList(QLatin1String("receiveAtcStationList"), argumentList);
            }

            inline QDBusPendingReply<> receiveCallsign(const BlackMisc::Aviation::CCallsign &callsign)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(callsign);
                return asyncCallWithArgumentList(QLatin1String("receiveCallsign"), argumentList);
            }

            inline QDBusPendingReply<> receiveComUnit(const BlackMisc::Aviation::CComSystem &comUnit)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(comUnit);
                return asyncCallWithArgumentList(QLatin1String("receiveComUnit"), argumentList);
            }

            inline QDBusPendingReply<> receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(geo);
                return asyncCallWithArgumentList(QLatin1String("receiveGeoPosition"), argumentList);
            }

            inline QDBusPendingReply<> receiveLength(const BlackMisc::PhysicalQuantities::CLength &length)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(length);
                return asyncCallWithArgumentList(QLatin1String("receiveLength"), argumentList);
            }

            inline QDBusPendingReply<> receiveLengthsQl(const BlackMisc::CVariantList &lengthsList)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(lengthsList);
                return asyncCallWithArgumentList(QLatin1String("receiveLengthsQl"), argumentList);
            }

            inline QDBusPendingReply<> receiveLengthsQvl(const BlackMisc::CVariantList &lengthsVariantList)
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

            inline QDBusPendingReply<> receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed)
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

            inline QDBusPendingReply<> receiveTransponder(const BlackMisc::Aviation::CTransponder &transponder)
            {
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(transponder);
                return asyncCallWithArgumentList(QLatin1String("receiveTransponder"), argumentList);
            }

            inline QDBusPendingReply<> receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap)
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
                const bool equal = (in == out);
                if (!equal) { errors++; }
                if (equal && !verbose) { return true; }
                ts << "I: " << in.toQString() << endl << "O: " << out.toQString() << endl;
                return equal;
            }

            //! Error info string
            static const QString &errorInfo(bool ok);
        };
    } // ns
} // ns
#endif
