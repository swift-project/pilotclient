/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_ownaircraft_proxy.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextOwnAircraftProxy::CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void CContextOwnAircraftProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                                    "changedAircraftCockpit", this, SIGNAL(changedAircraftCockpit(BlackMisc::Aviation::CAircraft, QString)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextOwnAircraft::ObjectPath(), IContextOwnAircraft::InterfaceName(),
                               "changedSelcal", this, SIGNAL(changedSelcal(BlackMisc::Aviation::CSelcal, QString)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    BlackMisc::Aviation::CAircraft CContextOwnAircraftProxy::getOwnAircraft() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("getOwnAircraft"));
    }

    void CContextOwnAircraftProxy::updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnAircraft"), aircraft, originator);
    }

    bool CContextOwnAircraftProxy::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateOwnPosition"), position, altitude, originator);
    }

    bool CContextOwnAircraftProxy::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateOwnSituation"), situation, originator);
    }

    bool CContextOwnAircraftProxy::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateOwnCockpit"), com1, com2, transponder, originator);
    }

    bool CContextOwnAircraftProxy::updatePilot(const BlackMisc::Network::CUser &pilot, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updatePilot"), pilot, originator);
    }

    bool CContextOwnAircraftProxy::updateSelcal(const CSelcal &selcal, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateSelcal"), selcal, originator);
    }

    bool CContextOwnAircraftProxy::updateCallsign(const CCallsign &callsign, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateCallsign"), callsign, originator);
    }

    bool CContextOwnAircraftProxy::updateIcaoData(const CAircraftIcao &icaoData, const QString &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateIcaoData"), icaoData, originator);
    }

    void CContextOwnAircraftProxy::setAudioOutputVolumes(int outputVolumeCom1, int outputVolumeCom2)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnCockpitOutputVolumes"), outputVolumeCom1, outputVolumeCom2);
    }

    void CContextOwnAircraftProxy::setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setAudioVoiceRoomOverrideUrls"), voiceRoom1Url, voiceRoom2Url);
    }

    void CContextOwnAircraftProxy::enableAutomaticVoiceRoomResolution(bool enable)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("enableAutomaticVoiceRoomResolution"), enable);
    }

    bool CContextOwnAircraftProxy::parseCommandLine(const QString &commandLine)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("parseCommandLine"), commandLine);
    }

} // namespace
