/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextownaircraftproxy.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Context
    {
        CContextOwnAircraftProxy::CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
        {
            this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
                serviceName , IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextOwnAircraftProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            bool s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                        "changedAircraftCockpit", this, SIGNAL(changedAircraftCockpit(BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::CIdentifier)));
            Q_ASSERT(s);
            Q_UNUSED(s);
            this->relayBaseClassSignals(serviceName, connection, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName());
        }

        void CContextOwnAircraftProxy::unitTestRelaySignals()
        {
            // connect signals, asserts when failures
            QDBusConnection con = QDBusConnection::sessionBus();
            CContextOwnAircraftProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
            Q_UNUSED(c);
        }

        BlackMisc::Simulation::CSimulatedAircraft CContextOwnAircraftProxy::getOwnAircraft() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraft>(QLatin1String("getOwnAircraft"));
        }

        bool CContextOwnAircraftProxy::updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateCockpit"), com1, com2, transponder, originator);
        }

        bool CContextOwnAircraftProxy::updateActiveComFrequency(const PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateActiveComFrequency"), frequency, comUnit, originator);
        }

        bool CContextOwnAircraftProxy::updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnAircraftPilot"), pilot);
        }

        bool CContextOwnAircraftProxy::updateSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateSelcal"), selcal, originator);
        }

        bool CContextOwnAircraftProxy::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const CAltitude &pressureAltitude)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnPosition"), position, altitude, pressureAltitude);
        }

        bool CContextOwnAircraftProxy::updateOwnCallsign(const CCallsign &callsign)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnCallsign"), callsign);
        }

        bool CContextOwnAircraftProxy::updateOwnIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("updateOwnIcaoCodes"), aircraftIcaoCode, airlineIcaoCode);
        }

        void CContextOwnAircraftProxy::setAudioOutputVolume(int outputVolume)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setAudioOutputVolume"), outputVolume);
        }

        void CContextOwnAircraftProxy::setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setAudioVoiceRoomOverrideUrls"), voiceRoom1Url, voiceRoom2Url);
        }

        void CContextOwnAircraftProxy::enableAutomaticVoiceRoomResolution(bool enable)
        {
            this->m_dBusInterface->callDBus(QLatin1String("enableAutomaticVoiceRoomResolution"), enable);
        }

        bool CContextOwnAircraftProxy::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("parseCommandLine"), commandLine, originator);
        }
    } // namespace
} // namespace
