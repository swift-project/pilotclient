// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/test/testserviceinterface.h"
#include "blackmisc/test/testservice.h"
#include "blackmisc/test/testing.h"
#include "blackmisc/test/testdata.h"
#include "blackmisc/aviation/flightplan.h"
#include <QTextStream>

class QDBusConnection;

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Test;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Test
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

        const CPropertyIndex pi({ 1000, 2000, 3000, 4000, 5000 }); // numbers >= global index
        const CPropertyIndex piPing = testServiceInterface.pingPropertyIndex(pi);
        ok = pingCompare(pi, piPing, out, verbose, errors);
        if (verbose) { out << "Pinged property index via interface" << errorInfo(ok) << Qt::endl; }

        const CPropertyIndexVariantMap ivm = CTestData::getCPropertyIndexVariantMap();
        const CPropertyIndexVariantMap ivmPing = testServiceInterface.pingIndexVariantMap(ivm);
        ok = pingCompare(ivm, ivmPing, out, verbose, errors);
        if (verbose) { out << "Pinged variant map via interface" << errorInfo(ok) << Qt::endl; }

        const CSimulatorPluginInfo pluginInfo("fsx", "FSX Simulator", "FSX", "Flight Simulator X", true);
        const CSimulatorPluginInfo pluginInfoPing = testServiceInterface.pingPluginInfo(pluginInfo);
        ok = pingCompare(pluginInfo, pluginInfoPing, out, verbose, errors);
        if (verbose) { out << "Pinged info via interface" << errorInfo(ok) << Qt::endl; }

        const CSpeed speedNotNull(22, CSpeedUnit::m_s());
        const CSpeed speedNull = CSpeed(0, CSpeedUnit::nullUnit());

        const CSpeed speedNotNullPing = testServiceInterface.pingSpeed(speedNotNull);
        ok = pingCompare(speedNotNull, speedNotNullPing, out, verbose, errors);
        if (verbose) { out << "Pinged not null speed via interface" << errorInfo(ok) << Qt::endl; }

        const CSpeed speedNullPing = testServiceInterface.pingSpeed(speedNull);
        ok = pingCompare(speedNull, speedNullPing, out, verbose, errors);
        if (verbose) { out << "Pinged null speed via interface" << errorInfo(ok) << Qt::endl; }

        const CAtcStation station = CTestData::getRandomAtcStation();
        const CAtcStation stationPing = testServiceInterface.pingAtcStation(station);
        ok = pingCompare(station, stationPing, out, verbose, errors);
        if (verbose) { out << "Pinged ATC station via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftIcaoCode icaoData = CTestData::getDBAircraftIcaoB737();
        const CAircraftIcaoCode icaoDataPing = testServiceInterface.pingAircraftIcaoData(icaoData);
        ok = pingCompare(icaoData, icaoDataPing, out, verbose, errors);
        if (verbose) { out << "Pinged ICAO data via interface" << errorInfo(ok) << Qt::endl; }

        const CUser user = CTestData::getRandomPilot();
        const CUser userPing = testServiceInterface.pingUser(user);
        ok = pingCompare(user, userPing, out, verbose, errors);
        if (verbose) { out << "Pinged user via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftSituation situation = CTestData::getAircraftSituationAboveMunichTower();
        const CAircraftSituation situationPing = testServiceInterface.pingSituation(situation);
        ok = pingCompare(situation, situationPing, out, verbose, errors);
        if (verbose) { out << "Pinged situation via interface" << errorInfo(ok) << Qt::endl; }

        const CTransponder transponder(1234, "C");
        const CTransponder transponderPing = testServiceInterface.pingTransponder(transponder);
        ok = pingCompare(transponder, transponderPing, out, verbose, errors);
        if (verbose) { out << "Pinged transponder via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftLights lights(true, false, true, false, true, false);
        const CAircraftLights lightsPing = testServiceInterface.pingAircraftLights(lights);
        ok = pingCompare(lights, lightsPing, out, verbose, errors);
        if (verbose) { out << "Pinged lights via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftEngine engine(2, false);
        const CAircraftEngine enginePing = testServiceInterface.pingAircraftEngine(engine);
        ok = pingCompare(engine, enginePing, out, verbose, errors);
        if (verbose) { out << "Pinged engine via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftEngineList engines({ engine });
        const CAircraftParts parts(lights, true, 11, true, engines, true);
        const CAircraftParts partsPing = testServiceInterface.pingAircraftParts(parts);
        ok = pingCompare(parts, partsPing, out, verbose, errors);
        if (verbose) { out << "Pinged engine via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftModel model = CTestData::getDbAircraftModelFsxAerosoftA320();
        const CAircraftModel modelPing = testServiceInterface.pingAircraftModel(model);
        ok = pingCompare(model, modelPing, out, verbose, errors);
        if (verbose) { out << "Pinged model via interface" << errorInfo(ok) << Qt::endl; }

        const CAircraftModelList models({ model, CTestData::getDbAircraftModelFsxA2AC172Skyhawk() });
        const CAircraftModelList modelsPing = testServiceInterface.pingAircraftModelList(models);
        ok = pingCompare(models, modelsPing, out, verbose, errors);
        if (verbose) { out << "Pinged model list via interface" << errorInfo(ok) << Qt::endl; }

        const CSimulatedAircraft aircraft = CTestData::getA320Aircraft();
        const CSimulatedAircraft aircraftPing = testServiceInterface.pingSimulatedAircraft(aircraft);
        ok = pingCompare(aircraft, aircraftPing, out, verbose, errors);
        if (verbose) { out << "Pinged simulated aircraft via interface" << errorInfo(ok) << Qt::endl; }

        const CAtcStationList atcStationList = CTestData::getAtcStations();
        const CAtcStationList atcStationListPing = testServiceInterface.pingAtcStationList(atcStationList);
        ok = pingCompare(atcStationList, atcStationListPing, out, verbose, errors);
        if (verbose) { out << "Pinged ATC station list via interface" << errorInfo(ok) << Qt::endl; }

        const CAirportList airportList = CTesting::getAirports(10);
        const CAirportList airportListPing = testServiceInterface.pingAirportList(airportList);
        ok = pingCompare(airportList, airportListPing, out, verbose, errors);
        if (verbose) { out << "Pinged airports list via interface" << errorInfo(ok) << Qt::endl; }

        const CClientList clients = CTesting::getClients(10);
        const CClient client = clients.front();
        const CClient clientPing = testServiceInterface.pingClient(client);
        ok = pingCompare(client, clientPing, out, verbose, errors);
        if (verbose) { out << "Pinged client list via interface" << errorInfo(ok) << Qt::endl; }

        const CClientList clientsPing = testServiceInterface.pingClientList(clients);
        ok = pingCompare(clients, clientsPing, out, verbose, errors);
        if (verbose) { out << "Pinged client list via interface" << errorInfo(ok) << Qt::endl; }

        CFlightPlan flightPlan;
        flightPlan.setEnrouteTime(CTime(4, CTimeUnit::h()));
        flightPlan.setFuelTime(CTime(5, CTimeUnit::h()));
        flightPlan.setCruiseAltitude(CAltitude(10, CAltitude::FlightLevel, CLengthUnit::km()));
        flightPlan.setCruiseTrueAirspeed(CSpeed(500, CSpeedUnit::km_h()));
        const CFlightPlan flightPlanPing = testServiceInterface.pingFlightPlan(flightPlan);
        ok = pingCompare(flightPlan, flightPlanPing, out, verbose, errors);
        if (verbose) { out << "Pinged flight plan via interface" << errorInfo(ok) << Qt::endl; }

        const CVariant cv = CVariant::fromValue(clients);
        const CVariant cvPing = testServiceInterface.pingCVariant(cv);
        ok = pingCompare(cv.value<CClientList>(), cvPing.value<CClientList>(), out, verbose, errors);
        if (verbose) { out << "Pinged CVariant(clients) list via interface" << errorInfo(ok) << Qt::endl; }

        const CVariant cv2 = CVariant::fromValue(aircraft);
        const CVariant cv2Ping = testServiceInterface.pingCVariant(cv2);
        ok = pingCompare(cv2.value<CSimulatedAircraft>(), cv2Ping.value<CSimulatedAircraft>(), out, verbose, errors);
        if (verbose) { out << "Pinged CVariant(aircraft) list via interface" << errorInfo(ok) << Qt::endl; }

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
