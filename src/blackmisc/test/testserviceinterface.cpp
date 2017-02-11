/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testserviceinterface.h"
#include "testservice.h"
#include "testutils.h"
#include "blackmisc/test/testdata.h"
#include <QTextStream>

class QDBusConnection;

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Test;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Test
    {
        ITestServiceInterface::ITestServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
            : QDBusAbstractInterface(service, path, ITestServiceInterface::InterfaceName(), connection, parent)
        {}

        ITestServiceInterface::~ITestServiceInterface()
        {}

        int ITestServiceInterface::pingTests(ITestServiceInterface &testServiceInterface, bool verbose)
        {
            int errors = 0;
            bool ok = false;
            QTextStream out(stdout);

            const CPropertyIndex pi({ 1000, 2000, 3000, 4000, 5000});  // numbers >= global index
            const CPropertyIndex piPing = testServiceInterface.pingPropertyIndex(pi);
            ok = pingCompare(pi, piPing, out, verbose, errors);
            if (verbose) { out << "Pinged property index via interface" << errorInfo(ok) << endl; }

            CPropertyIndexVariantMap ivm;
            ivm.addValue(1000, "one");
            ivm.addValue(2000, "two");
            ivm.addValue(3000, "three");
            const CPropertyIndexVariantMap ivmPing = testServiceInterface.pingIndexVariantMap(ivm);
            ok = pingCompare(ivm, ivmPing, out, verbose, errors);
            if (verbose) { out << "Pinged variant map via interface" << errorInfo(ok) << endl; }

            const CSimulatorPluginInfo pluginInfo("fsx", "FSX Simulator", "FSX", "Flight Simulator X", true);
            const CSimulatorPluginInfo pluginInfoPing = testServiceInterface.pingPluginInfo(pluginInfo);
            ok = pingCompare(pluginInfo, pluginInfoPing, out, verbose, errors);
            if (verbose) { out << "Pinged info via interface" << errorInfo(ok) << endl; }

            const CSpeed speedNotNull(22, CSpeedUnit::m_s());
            const CSpeed speedNull = CSpeed(0, CSpeedUnit::nullUnit());

            const CSpeed speedNotNullPing = testServiceInterface.pingSpeed(speedNotNull);
            ok = pingCompare(speedNotNull, speedNotNullPing, out, verbose, errors);
            if (verbose) { out << "Pinged not null speed via interface" << errorInfo(ok) << endl; }

            const CSpeed speedNullPing = testServiceInterface.pingSpeed(speedNull);
            ok = pingCompare(speedNull, speedNullPing, out, verbose, errors);
            if (verbose) { out << "Pinged null speed via interface" << errorInfo(ok) << endl; }

            const CAtcStation station = CTestData::getAtcStation();
            const CAtcStation stationPing = testServiceInterface.pingAtcStation(station);
            ok = pingCompare(station, stationPing, out, verbose, errors);
            if (verbose) { out << "Pinged ATC station via interface" << errorInfo(ok) << endl; }

            const CAircraftIcaoCode icaoData("B737", "L2J");
            const CAircraftIcaoCode icaoDataPing = testServiceInterface.pingAircraftIcaoData(icaoData);
            ok = pingCompare(icaoData, icaoDataPing, out, verbose, errors);
            if (verbose) { out << "Pinged ICAO data via interface" << errorInfo(ok) << endl; }

            const CUser user("223344", "Ping Me User");
            const CUser userPing = testServiceInterface.pingUser(user);
            ok = pingCompare(user, userPing, out, verbose, errors);
            if (verbose) { out << "Pinged user via interface" << errorInfo(ok) << endl; }

            // EDDF: 50° 2′ 0″ N, 8° 34′ 14″ E, 2000m MSL
            const CCoordinateGeodetic coordinate = CCoordinateGeodetic::fromWgs84("50° 2′ 1″ 23 N", "8° 34′ 14″ E", { 2000, CLengthUnit::m() });
            const CAircraftSituation situation = CAircraftSituation("DAMBZ", coordinate);
            const CAircraftSituation situationPing = testServiceInterface.pingSituation(situation);
            ok = pingCompare(situation, situationPing, out, verbose, errors);
            if (verbose) { out << "Pinged situation via interface" << errorInfo(ok) << endl; }

            const CTransponder transponder(1234, "C");
            const CTransponder transponderPing = testServiceInterface.pingTransponder(transponder);
            ok = pingCompare(transponder, transponderPing, out, verbose, errors);
            if (verbose) { out << "Pinged transponder via interface" << errorInfo(ok) << endl; }

            const CAircraftLights lights(true, false, true, false, true, false);
            const CAircraftLights lightsPing = testServiceInterface.pingAircraftLights(lights);
            ok = pingCompare(lights, lightsPing, out, verbose, errors);
            if (verbose) { out << "Pinged lights via interface" << errorInfo(ok) << endl; }

            const CAircraftEngine engine(2, false);
            const CAircraftEngine enginePing = testServiceInterface.pingAircraftEngine(engine);
            ok = pingCompare(engine, enginePing, out, verbose, errors);
            if (verbose) { out << "Pinged engine via interface" << errorInfo(ok) << endl; }

            const CAircraftEngineList engines({engine});
            const CAircraftParts parts(lights, true, 11, true, engines, true);
            const CAircraftParts partsPing = testServiceInterface.pingAircraftParts(parts);
            ok = pingCompare(parts, partsPing, out, verbose, errors);
            if (verbose) { out << "Pinged engine via interface" << errorInfo(ok) << endl; }

            const CAircraftModel model("foobar", CAircraftModel::TypeManuallySet);
            const CAircraftModel modelPing = testServiceInterface.pingAircraftModel(model);
            ok = pingCompare(model, modelPing, out, verbose, errors);
            if (verbose) { out << "Pinged model via interface" << errorInfo(ok) << endl; }

            const CAircraftModel model2("mymodel", CAircraftModel::TypeFSInnData);
            const CAircraftModelList models({ model, model2});
            const CAircraftModelList modelsPing = testServiceInterface.pingAircraftModelList(models);
            ok = pingCompare(models, modelsPing, out, verbose, errors);
            if (verbose) { out << "Pinged model list via interface" << errorInfo(ok) << endl; }

            const CCallsign callsign("DEMBZ");
            CSimulatedAircraft aircraft(callsign, CUser("123456", "Joe Pilot"), situation);
            aircraft.setTransponder(transponder);
            aircraft.setModel(model);
            aircraft.setNetworkModel(model2);
            const CSimulatedAircraft aircraftPing = testServiceInterface.pingSimulatedAircraft(aircraft);
            ok = pingCompare(aircraft, aircraftPing, out, verbose, errors);
            pingCompare(aircraft.getModel(), aircraftPing.getModel(), out, verbose, errors);
            pingCompare(aircraft.getNetworkModel(), aircraftPing.getNetworkModel(), out, verbose, errors);
            if (verbose) { out << "Pinged simulated aircraft via interface" << errorInfo(ok) << endl; }

            CAtcStationList atcStationList;
            atcStationList.push_back(station);
            atcStationList.push_back(station);
            atcStationList.push_back(station);
            CAtcStationList atcStationListPing = testServiceInterface.pingAtcStationList(atcStationList);
            ok = pingCompare(atcStationList, atcStationListPing, out, verbose, errors);
            if (verbose) { out << "Pinged ATC station list via interface" << errorInfo(ok) << endl; }

            const CAirportList airportList = CTestUtils::getAirports(10);
            const CAirportList airportListPing = testServiceInterface.pingAirportList(airportList);
            ok = pingCompare(airportList, airportListPing, out, verbose, errors);
            if (verbose) { out << "Pinged airports list via interface" << errorInfo(ok) << endl; }

            const CClientList clients = CTestUtils::getClients(10);
            const CClient client = clients.front();
            const CClient clientPing = testServiceInterface.pingClient(client);
            ok = pingCompare(client, clientPing, out, verbose, errors);
            if (verbose) { out << "Pinged client list via interface" << errorInfo(ok) << endl; }

            const CClientList clientsPing = testServiceInterface.pingClientList(clients);
            ok = pingCompare(clients, clientsPing, out, verbose, errors);
            if (verbose) { out << "Pinged client list via interface" << errorInfo(ok) << endl; }

            const CVariant cv = CVariant::fromValue(clients);
            const CVariant cvPing = testServiceInterface.pingCVariant(cv);
            ok = pingCompare(cv.value<CClientList>(), cvPing.value<CClientList>(), out, verbose, errors);
            if (verbose) { out << "Pinged CVariant(clients) list via interface" << errorInfo(ok) << endl; }

            const CVariant cv2 = CVariant::fromValue(aircraft);
            const CVariant cvPing2 = testServiceInterface.pingCVariant(cv2);
            ok = pingCompare(cv2.value<CSimulatedAircraft>(), cvPing2.value<CSimulatedAircraft>(), out, verbose, errors);
            if (verbose) { out << "Pinged CVariant(aircraft) list via interface" << errorInfo(ok) << endl; }

            // end
            return errors;
        }

        const char *ITestServiceInterface::InterfaceName()
        {
            static const QByteArray in(CTestService::InterfaceName().toLatin1());
            return in.constData();
        }

        const QString &ITestServiceInterface::errorInfo(bool ok)
        {
            static const QString sOk(": ok");
            static const QString sError(": !! ERROR !!");
            return ok ? sOk : sError;
        }
    } // ns
} // ns
