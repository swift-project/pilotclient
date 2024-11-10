// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/test/testservice.h"
#include "misc/test/testing.h"
#include "misc/dbusutils.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/track.h"
#include "misc/dbus.h"
#include "misc/pq/frequency.h"
#include "misc/pq/units.h"
#include "misc/stringutils.h"

#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QDBusError>
#include <QDebug>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::network;

namespace swift::misc::test
{
    CTestService::CTestService(QObject *parent) : QObject(parent)
    {}

    CTestService::CTestService(bool verbose, QObject *parent) : QObject(parent), m_verbose(verbose)
    {}

    bool CTestService::canRegisterTestService(QDBusConnection &connection)
    {
        const bool r = connection.registerService(CTestService::InterfaceName());
        if (r)
        {
            connection.unregisterService(CTestService::InterfaceName());
        }
        return r;
    }

    const QString &CTestService::InterfaceName()
    {
        static const QString i(SWIFT_MISC_TESTSERVICE_INTERFACENAME);
        return i;
    }

    const QString &CTestService::ObjectPath()
    {
        static const QString p("/ts");
        return p;
    }

    CTestService *CTestService::registerTestService(QDBusConnection &connection, bool verbose, QObject *parent)
    {
        CTestService *pTestService = new CTestService(verbose, parent); // just a QObject with signals / slots and  Q_CLASSINFO("D-Bus Interface", some service name)
        if (!connection.registerService(CTestService::InterfaceName()))
        {
            QDBusError error = connection.lastError();
            err() << error.message();
            err() << "Started dbus-daemon.exe --session (Windows)?" << Qt::endl;
            err() << "Created directory session.d (e.g. ../Qt/5.8.0/qtbase/etc/dbus-1/session.d)?" << Qt::endl;
            qFatal("Could not register service!");
        }

        if (!connection.registerObject(CTestService::ObjectPath(), pTestService, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAdaptors))
        {
            qFatal("Could not register service object!");
        }

        out() << "Registration running as pid: " << CTestService::getPid() << Qt::endl;
        if (pTestService) { out() << "Service registered" << Qt::endl; }

        QString service; // service not needed
        if (connection.connect(service, CTestService::ObjectPath(), CTestService::InterfaceName(),
                               "sendStringMessage", pTestService, SLOT(receiveStringMessage(const QString &))))
        {
            out() << "Connected object with DBus 'sendStringMessage'" << Qt::endl;
        }
        else
        {
            qFatal("Cannot connect service with DBus");
        }
        return pTestService;
    }

    bool CTestService::unregisterTestService(QDBusConnection &connection)
    {
        return connection.unregisterService(CTestService::InterfaceName());
    }

    QTextStream &CTestService::out()
    {
        static QTextStream out(stdout);
        return out;
    }

    QTextStream &CTestService::err()
    {
        static QTextStream err(stderr);
        return err;
    }

    void CTestService::receiveStringMessage(const QString &message) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received message: " << message << Qt::endl;
    }

    void CTestService::receiveVariant(const CVariant &variant) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received variant: " << variant << Qt::endl;
    }

    void CTestService::receiveSpeed(const swift::misc::physical_quantities::CSpeed &speed) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received speed: " << speed << Qt::endl;
    }

    void CTestService::receiveComUnit(const CComSystem &comUnit) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received COM: " << comUnit << Qt::endl;
    }

    void CTestService::receiveAltitude(const CAltitude &altitude) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received altitude: " << altitude << Qt::endl;
    }

    void CTestService::receiveList(const QList<double> &list) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received list: " << list.size() << Qt::endl;
    }

    void CTestService::receiveGeoPosition(const swift::misc::geo::CCoordinateGeodetic &geo) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received geo data: " << geo << Qt::endl;
    }

    void CTestService::receiveTransponder(const CTransponder &transponder) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received transponder: " << transponder << Qt::endl;
    }

    void CTestService::receiveTrack(const CTrack &track) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received track: " << track << Qt::endl;
    }

    void CTestService::receiveLength(const swift::misc::physical_quantities::CLength &length) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received length: " << length << Qt::endl;
    }

    void CTestService::receiveVariantList(const CVariantList &variantList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " " << variantList.size() << Qt::endl;
        for (const CVariant &lv : variantList)
        {
            if (m_verbose) out() << "    Received variant: " << lv.toQString() << Qt::endl;
        }
    }

    void CTestService::receiveCallsign(const CCallsign &callsign) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received callsign: " << callsign << Qt::endl;
    }

    void CTestService::receiveAtcStationList(const CAtcStationList &atcStationList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received ATC list: " << atcStationList << Qt::endl;
    }

    void CTestService::receiveValueMap(const swift::misc::CPropertyIndexVariantMap &valueMap) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received value map: " << valueMap << Qt::endl;
    }

    void CTestService::receiveAtcStation(const CAtcStation &station) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received ATC station: " << station << Qt::endl;
    }

    CAtcStationList CTestService::pingAtcStationList(const CAtcStationList &atcStationList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ATCs: " << atcStationList << Qt::endl;
        return atcStationList;
    }

    CSimulatedAircraftList CTestService::pingAircraftList(const CSimulatedAircraftList &aircraftList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft: " << aircraftList << Qt::endl;
        return aircraftList;
    }

    CAircraftParts CTestService::pingAircraftParts(const CAircraftParts &aircraftParts) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft parts: " << aircraftParts << Qt::endl;
        return aircraftParts;
    }

    CAircraftEngine CTestService::pingAircraftEngine(const CAircraftEngine &aircraftEngine) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft engine: " << aircraftEngine << Qt::endl;
        return aircraftEngine;
    }

    CAircraftModel CTestService::pingAircraftModel(const CAircraftModel &aircraftModel) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft model: " << aircraftModel << Qt::endl;
        return aircraftModel;
    }

    CAircraftModelList CTestService::pingAircraftModelList(const CAircraftModelList &aircraftModels) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft modellist: " << aircraftModels << Qt::endl;
        return aircraftModels;
    }

    CAircraftLights CTestService::pingAircraftLights(const CAircraftLights &aircraftLights) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft lights: " << aircraftLights << Qt::endl;
        return aircraftLights;
    }

    CSimulatedAircraft CTestService::pingSimulatedAircraft(const CSimulatedAircraft &aircraft) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping simulated aircraft: " << aircraft << Qt::endl;
        return aircraft;
    }

    CAirportList CTestService::pingAirportList(const CAirportList &airportList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping airports: " << airportList << Qt::endl;
        return airportList;
    }

    CPropertyIndex CTestService::pingPropertyIndex(const CPropertyIndex &properties) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping properties: " << properties << Qt::endl;
        return properties;
    }

    CPropertyIndexVariantMap CTestService::pingIndexVariantMap(const CPropertyIndexVariantMap &indexVariantMap) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping properties: " << indexVariantMap << Qt::endl;
        return indexVariantMap;
    }

    CClient CTestService::pingClient(const CClient &client) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping client: " << client << Qt::endl;
        return client;
    }

    CClientList CTestService::pingClientList(const CClientList &clientList) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping clients: " << clientList << Qt::endl;
        return clientList;
    }

    CSpeed CTestService::pingSpeed(const CSpeed &speed) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping speed: " << speed << Qt::endl;
        return speed;
    }

    CAltitude CTestService::pingAltitude(const CAltitude &altitude) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping alt: " << altitude << Qt::endl;
        return altitude;
    }

    CUser CTestService::pingUser(const CUser &user) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping user: " << user << Qt::endl;
        return user;
    }

    CAircraftSituation CTestService::pingSituation(const CAircraftSituation &situation) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping situation: " << situation << Qt::endl;
        return situation;
    }

    CTransponder CTestService::pingTransponder(const CTransponder &transponder) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping transponder: " << transponder << Qt::endl;
        return transponder;
    }

    CAtcStation CTestService::pingAtcStation(const CAtcStation &station) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ATC: " << station << Qt::endl;
        return station;
    }

    CAircraftIcaoCode CTestService::pingAircraftIcaoData(const CAircraftIcaoCode &icao) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ICAO data: " << icao << Qt::endl;
        return icao;
    }

    CSimulatorPluginInfo CTestService::pingPluginInfo(const CSimulatorPluginInfo &info) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " info: " << info << Qt::endl;
        return info;
    }

    swift::misc::CVariant CTestService::pingCVariant(const CVariant &variant) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " client sent back as CVariant: " << variant.toQString() << Qt::endl;
        return variant;
    }

    swift::misc::aviation::CFlightPlan CTestService::pingFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " info: " << flightPlan << Qt::endl;
        return flightPlan;
    }

    CAtcStationList CTestService::getAtcStationList(int n) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getAtcStationList" << Qt::endl;
        return CTesting::createAtcStations(n, false);
    }

    CAircraftCfgEntriesList CTestService::getAircraftCfgEntriesList(int n) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getAircraftCfgEntriesList" << Qt::endl;
        return CTesting::getAircraftCfgEntries(n);
    }

    QList<QDBusObjectPath> CTestService::getObjectPaths(int n) const
    {
        if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getObjectPaths" << Qt::endl;
        QList<QDBusObjectPath> paths;
        paths.reserve(n);
        for (int i = 0; i < n; i++)
        {
            paths.append(QDBusObjectPath(ObjectPath()));
        }
        return paths;
    }
} // ns
