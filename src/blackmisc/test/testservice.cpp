/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "testservice.h"
#include "testing.h"
#include "blackmisc/dbusutils.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/track.h"
#include "blackmisc/dbus.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/stringutils.h"

#include <QDBusArgument>
#include <QDBusObjectPath>
#include <QDBusError>
#include <QDebug>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Test
    {
        CTestService::CTestService(QObject *parent) : QObject(parent)
        { }

        CTestService::CTestService(bool verbose, QObject *parent) :
            QObject(parent), m_verbose(verbose)
        { }

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
            static const QString i(BLACKMISC_TESTSERVICE_INTERFACENAME);
            return i;
        }

        const QString &CTestService::ObjectPath()
        {
            static const QString p("/ts");
            return p;
        }

        CTestService *CTestService::registerTestService(QDBusConnection &connection, bool verbose, QObject *parent)
        {
            CTestService *pTestService = new CTestService(verbose, parent);  // just a QObject with signals / slots and  Q_CLASSINFO("D-Bus Interface", some service name)
            if (!connection.registerService(CTestService::InterfaceName()))
            {
                QDBusError error = connection.lastError();
                err() << error.message();
                err() << "Started dbus-daemon.exe --session (Windows)?" << endl;
                err() << "Created directory session.d (e.g. ../Qt/5.8.0/qtbase/etc/dbus-1/session.d)?" << endl;
                qFatal("Could not register service!");
            }

            if (!connection.registerObject(CTestService::ObjectPath(), pTestService, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAdaptors))
            {
                qFatal("Could not register service object!");
            }

            out() << "Registration running as pid: " << CTestService::getPid() << endl;
            if (pTestService) { out() << "Service registered" << endl; }

            QString service; // service not needed
            if (connection.connect(service, CTestService::ObjectPath(), CTestService::InterfaceName(),
                                   "sendStringMessage", pTestService, SLOT(receiveStringMessage(const QString &))))
            {
                out() << "Connected object with DBus 'sendStringMessage'" << endl;
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
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received message: " << message << endl;
        }

        void CTestService::receiveVariant(const CVariant &variant) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received variant: " << variant << endl;
        }

        void CTestService::receiveSpeed(const BlackMisc::PhysicalQuantities::CSpeed &speed) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received speed: " << speed << endl;
        }

        void CTestService::receiveComUnit(const CComSystem &comUnit) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received COM: " << comUnit << endl;
        }

        void CTestService::receiveAltitude(const CAltitude &altitude) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received altitude: " << altitude << endl;
        }

        void CTestService::receiveList(const QList<double> &list) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received list: " << list.size() << endl;
        }

        void CTestService::receiveGeoPosition(const BlackMisc::Geo::CCoordinateGeodetic &geo) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received geo data: " << geo << endl;
        }

        void CTestService::receiveTransponder(const CTransponder &transponder) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received transponder: " << transponder << endl;
        }

        void CTestService::receiveTrack(const CTrack &track) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received track: " << track << endl;
        }

        void CTestService::receiveLength(const BlackMisc::PhysicalQuantities::CLength &length) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received length: " << length << endl;
        }

        void CTestService::receiveVariantList(const CVariantList &variantList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " " << variantList.size() << endl;
            for (const CVariant &lv : variantList)
            {
                if (m_verbose) out() << "    Received variant: " << lv.toQString() << endl;
            }
        }

        void CTestService::receiveCallsign(const CCallsign &callsign) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received callsign: " << callsign << endl;
        }

        void CTestService::receiveAtcStationList(const CAtcStationList &atcStationList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received ATC list: " << atcStationList << endl;
        }

        void CTestService::receiveValueMap(const BlackMisc::CPropertyIndexVariantMap &valueMap) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received value map: " << valueMap << endl;
        }

        void CTestService::receiveAtcStation(const CAtcStation &station) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " Received ATC station: " << station << endl;
        }

        CAtcStationList CTestService::pingAtcStationList(const CAtcStationList &atcStationList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ATCs: " << atcStationList << endl;
            return atcStationList;
        }

        CSimulatedAircraftList CTestService::pingAircraftList(const CSimulatedAircraftList &aircraftList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft: " << aircraftList << endl;
            return aircraftList;
        }

        CAircraftParts CTestService::pingAircraftParts(const CAircraftParts &aircraftParts) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft parts: " << aircraftParts << endl;
            return aircraftParts;
        }

        CAircraftEngine CTestService::pingAircraftEngine(const CAircraftEngine &aircraftEngine) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft engine: " << aircraftEngine << endl;
            return aircraftEngine;
        }

        CAircraftModel CTestService::pingAircraftModel(const CAircraftModel &aircraftModel) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft model: " << aircraftModel << endl;
            return aircraftModel;
        }

        CAircraftModelList CTestService::pingAircraftModelList(const CAircraftModelList &aircraftModels) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft modellist: " << aircraftModels << endl;
            return aircraftModels;
        }

        CAircraftLights CTestService::pingAircraftLights(const CAircraftLights &aircraftLights) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping aircraft lights: " << aircraftLights << endl;
            return aircraftLights;
        }

        CSimulatedAircraft CTestService::pingSimulatedAircraft(const CSimulatedAircraft &aircraft) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping simulated aircraft: " << aircraft << endl;
            return aircraft;
        }

        CAirportList CTestService::pingAirportList(const CAirportList &airportList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping airports: " << airportList << endl;
            return airportList;
        }

        CPropertyIndex CTestService::pingPropertyIndex(const CPropertyIndex &properties) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping properties: " << properties << endl;
            return properties;
        }

        CPropertyIndexVariantMap CTestService::pingIndexVariantMap(const CPropertyIndexVariantMap &indexVariantMap) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping properties: " << indexVariantMap << endl;
            return indexVariantMap;
        }

        CClient CTestService::pingClient(const CClient &client) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping client: " << client << endl;
            return client;
        }

        CClientList CTestService::pingClientList(const CClientList &clientList) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping clients: " << clientList << endl;
            return clientList;
        }

        CSpeed CTestService::pingSpeed(const CSpeed &speed) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping speed: " << speed << endl;
            return speed;
        }

        CAltitude CTestService::pingAltitude(const CAltitude &altitude) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping alt: " << altitude << endl;
            return altitude;
        }

        CUser CTestService::pingUser(const CUser &user) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping user: " << user << endl;
            return user;
        }

        CAircraftSituation CTestService::pingSituation(const CAircraftSituation &situation) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping situation: " << situation << endl;
            return situation;
        }

        CTransponder CTestService::pingTransponder(const CTransponder &transponder) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping transponder: " << transponder << endl;
            return transponder;
        }

        CAtcStation CTestService::pingAtcStation(const CAtcStation &station) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ATC: " << station << endl;
            return station;
        }

        CAircraftIcaoCode CTestService::pingAircraftIcaoData(const CAircraftIcaoCode &icao) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " ping ICAO data: " << icao << endl;
            return icao;
        }

        CSimulatorPluginInfo CTestService::pingPluginInfo(const CSimulatorPluginInfo &info) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " info: " << info << endl;
            return info;
        }

        BlackMisc::CVariant CTestService::pingCVariant(const CVariant &variant) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " client sent back as CVariant: " << variant.toQString() << endl;
            return variant;
        }

        BlackMisc::Aviation::CFlightPlan CTestService::pingFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " info: " << flightPlan << endl;
            return flightPlan;
        }

        CAtcStationList CTestService::getAtcStationList(int n) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getAtcStationList" << endl;
            return CTesting::createAtcStations(n, false);
        }

        CAircraftCfgEntriesList CTestService::getAircraftCfgEntriesList(int n) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getAircraftCfgEntriesList" << endl;
            return CTesting::getAircraftCfgEntries(n);
        }

        QList<QDBusObjectPath> CTestService::getObjectPaths(int n) const
        {
            if (m_verbose) out() << "Pid: " << CTestService::getPid() << " getObjectPaths" << endl;
            QList<QDBusObjectPath> paths;
            paths.reserve(n);
            for (int i = 0; i < n; i++)
            {
                paths.append(QDBusObjectPath(ObjectPath()));
            }
            return paths;
        }
    } // ns
} // ns
