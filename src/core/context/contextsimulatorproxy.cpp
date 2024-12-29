// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextsimulatorproxy.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>

#include "misc/dbus.h"
#include "misc/dbusserver.h"
#include "misc/genericdbusinterface.h"
#include "misc/simulation/simulatedaircraft.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::geo;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;

namespace swift::core::context
{
    CContextSimulatorProxy::CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection,
                                                   CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
        : IContextSimulator(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new swift::misc::CGenericDBusInterface(
            serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);
    }

    void CContextSimulatorProxy::unitTestRelaySignals()
    {
        // connect signals, asserts when failures
        QDBusConnection con = QDBusConnection::sessionBus();
        CContextSimulatorProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
        Q_UNUSED(c)
    }

    void CContextSimulatorProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        // the types here have to be fully qualifed

        bool s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                                    "simulatorStatusChanged", this, SIGNAL(simulatorStatusChanged(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "simulatorPluginChanged", this,
                               SIGNAL(simulatorPluginChanged(swift::misc::simulation::CSimulatorPluginInfo)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "simulatorChanged", this,
                               SIGNAL(simulatorChanged(swift::misc::simulation::CSimulatorInfo)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "simulatorSettingsChanged", this, SIGNAL(simulatorSettingsChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "vitalityLost", this, SIGNAL(vitalityLost()));
        Q_ASSERT(s);
        s = connection.connect(
            serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
            "renderRestrictionsChanged", this,
            SIGNAL(renderRestrictionsChanged(bool, bool, int, swift::misc::physical_quantities::CLength)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "interpolationAndRenderingSetupChanged", this,
                               SIGNAL(interpolationAndRenderingSetupChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "matchingSetupChanged", this, SIGNAL(matchingSetupChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "modelSetChanged", this,
                               SIGNAL(modelSetChanged(swift::misc::simulation::CSimulatorInfo)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "modelMatchingCompleted", this,
                               SIGNAL(modelMatchingCompleted(swift::misc::simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "addingRemoteModelFailed", this,
                               SIGNAL(addingRemoteModelFailed(swift::misc::simulation::CSimulatedAircraft, bool, bool,
                                                              swift::misc::CStatusMessage)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "aircraftRenderingChanged", this,
                               SIGNAL(aircraftRenderingChanged(swift::misc::simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "ownAircraftModelChanged", this,
                               SIGNAL(ownAircraftModelChanged(swift::misc::simulation::CAircraftModel)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "airspaceSnapshotHandled", this, SIGNAL(airspaceSnapshotHandled()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "driverMessages", this, SIGNAL(driverMessages(swift::misc::CStatusMessageList)));

        Q_ASSERT(s);
        s = connection.connect(
            serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(), "validatedModelSet", this,
            SIGNAL(
                validatedModelSet(swift::misc::simulation::CSimulatorInfo, swift::misc::simulation::CAircraftModelList,
                                  swift::misc::simulation::CAircraftModelList, bool, swift::misc::CStatusMessageList)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
                               "insufficientFrameRateDetected", this, SIGNAL(insufficientFrameRateDetected(bool)));
        Q_ASSERT(s);
        Q_UNUSED(s)
        this->relayBaseClassSignals(serviceName, connection, IContextSimulator::ObjectPath(),
                                    IContextSimulator::InterfaceName());
    }

    CSimulatorPluginInfoList CContextSimulatorProxy::getAvailableSimulatorPlugins() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorPluginInfoList>(QLatin1String("getAvailableSimulatorPlugins"));
    }

    CSimulatorSettings CContextSimulatorProxy::getSimulatorSettings() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorSettings>(QLatin1String("getSimulatorSettings"));
    }

    bool CContextSimulatorProxy::setSimulatorSettings(const CSimulatorSettings &settings,
                                                      const CSimulatorInfo &simulatorInfo)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("setSimulatorSettings"), settings, simulatorInfo);
    }

    ISimulator::SimulatorStatus CContextSimulatorProxy::getSimulatorStatus() const
    {
        return m_dBusInterface->callDBusRet<ISimulator::SimulatorStatus>(QLatin1String("getSimulatorStatus"));
    }

    CAirportList CContextSimulatorProxy::getAirportsInRange(bool recalculatePosition) const
    {
        return m_dBusInterface->callDBusRet<CAirportList>(QLatin1String("getAirportsInRange"), recalculatePosition);
    }

    CAircraftModelList CContextSimulatorProxy::getModelSet() const
    {
        return m_dBusInterface->callDBusRet<CAircraftModelList>(QLatin1String("getModelSet"));
    }

    CSimulatorInfo CContextSimulatorProxy::simulatorsWithInitializedModelSet() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorInfo>(QLatin1String("simulatorsWithInitializedModelSet"));
    }

    CStatusMessageList CContextSimulatorProxy::verifyPrerequisites() const
    {
        return m_dBusInterface->callDBusRet<swift::misc::CStatusMessageList>(QLatin1String("verifyPrerequisites"));
    }

    CSimulatorInfo CContextSimulatorProxy::getModelSetLoaderSimulator() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorInfo>(QLatin1String("getModelSetLoaderSimulator"));
    }

    void CContextSimulatorProxy::setModelSetLoaderSimulator(const CSimulatorInfo &simulator)
    {
        m_dBusInterface->callDBus(QLatin1String("setModelSetLoaderSimulator"), simulator);
    }

    QStringList CContextSimulatorProxy::getModelSetStrings() const
    {
        return m_dBusInterface->callDBusRet<QStringList>(QLatin1String("getModelSetStrings"));
    }

    QStringList CContextSimulatorProxy::getModelSetCompleterStrings(bool sorted) const
    {
        return m_dBusInterface->callDBusRet<QStringList>(QLatin1String("getModelSetCompleterStrings"), sorted);
    }

    int CContextSimulatorProxy::removeModelsFromSet(const CAircraftModelList &removeModels)
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1String("removeModelsFromSet"), removeModels);
    }

    bool CContextSimulatorProxy::isKnownModelInSet(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("isKnownModelInSet"), modelString);
    }

    CAircraftModelList CContextSimulatorProxy::getModelSetModelsStartingWith(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<CAircraftModelList>(QLatin1String("getModelSetModelsStartingWith"),
                                                                modelString);
    }

    int CContextSimulatorProxy::getModelSetCount() const
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1String("getModelSetCount"));
    }

    CSimulatorPluginInfo CContextSimulatorProxy::getSimulatorPluginInfo() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorPluginInfo>(QLatin1String("getSimulatorPluginInfo"));
    }

    CSimulatorInternals CContextSimulatorProxy::getSimulatorInternals() const
    {
        return m_dBusInterface->callDBusRet<CSimulatorInternals>(QLatin1String("getSimulatorInternals"));
    }

    void CContextSimulatorProxy::disableModelsForMatching(const CAircraftModelList &removedModels, bool incremental)
    {
        m_dBusInterface->callDBus(QLatin1String("disableModelsForMatching"), removedModels, incremental);
    }

    CAircraftModelList CContextSimulatorProxy::getDisabledModelsForMatching() const
    {
        return m_dBusInterface->callDBusRet<CAircraftModelList>(QLatin1String("getDisabledModelsForMatching"));
    }

    bool CContextSimulatorProxy::triggerModelSetValidation(const CSimulatorInfo &simulator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("triggerModelSetValidation"), simulator);
    }

    bool CContextSimulatorProxy::isValidationInProgress() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("isValidationInProgress"));
    }

    void CContextSimulatorProxy::restoreDisabledModels()
    {
        m_dBusInterface->callDBus(QLatin1String("restoreDisabledModels"));
    }

    bool CContextSimulatorProxy::setTimeSynchronization(bool enable, const CTime &offset)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("setTimeSynchronization"), enable, offset);
    }

    bool CContextSimulatorProxy::isTimeSynchronized() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("isTimeSynchronized"));
    }

    CInterpolationAndRenderingSetupGlobal CContextSimulatorProxy::getInterpolationAndRenderingSetupGlobal() const
    {
        return m_dBusInterface->callDBusRet<CInterpolationAndRenderingSetupGlobal>(
            QLatin1String("getInterpolationAndRenderingSetupGlobal"));
    }

    CInterpolationSetupList CContextSimulatorProxy::getInterpolationAndRenderingSetupsPerCallsign() const
    {
        return m_dBusInterface->callDBusRet<CInterpolationSetupList>(
            QLatin1String("getInterpolationAndRenderingSetupsPerCallsign"));
    }

    CInterpolationAndRenderingSetupPerCallsign
    CContextSimulatorProxy::getInterpolationAndRenderingSetupPerCallsignOrDefault(const CCallsign &callsign) const
    {
        return m_dBusInterface->callDBusRet<CInterpolationAndRenderingSetupPerCallsign>(
            QLatin1String("getInterpolationAndRenderingSetupPerCallsignOrDefault"), callsign);
    }

    bool CContextSimulatorProxy::setInterpolationAndRenderingSetupsPerCallsign(const CInterpolationSetupList &setups,
                                                                               bool ignoreSameAsGlobal)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("setInterpolationAndRenderingSetupsPerCallsign"),
                                                  setups, ignoreSameAsGlobal);
    }

    CStatusMessageList CContextSimulatorProxy::getInterpolationMessages(const CCallsign &callsign) const
    {
        return m_dBusInterface->callDBusRet<CStatusMessageList>(QLatin1String("getInterpolationMessages"), callsign);
    }

    void
    CContextSimulatorProxy::setInterpolationAndRenderingSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
    {
        m_dBusInterface->callDBus(QLatin1String("setInterpolationAndRenderingSetupGlobal"), setup);
    }

    CTime CContextSimulatorProxy::getTimeSynchronizationOffset() const
    {
        return m_dBusInterface->callDBusRet<swift::misc::physical_quantities::CTime>(
            QLatin1String("getTimeSynchronizationOffset"));
    }

    bool CContextSimulatorProxy::startSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("startSimulatorPlugin"), simulatorInfo);
    }

    void CContextSimulatorProxy::stopSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        m_dBusInterface->callDBus(QLatin1String("stopSimulatorPlugin"), simulatorInfo);
    }

    int CContextSimulatorProxy::checkListeners()
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1String("checkListeners"));
    }

    CPixmap CContextSimulatorProxy::iconForModel(const QString &modelString) const
    {
        return m_dBusInterface->callDBusRet<CPixmap>(QLatin1String("iconForModel"), modelString);
    }

    bool CContextSimulatorProxy::followAircraft(const CCallsign &callsign)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("followAircraft"), callsign);
    }

    void CContextSimulatorProxy::recalculateAllAircraft()
    {
        m_dBusInterface->callDBus(QLatin1String("recalculateAllAircraft"));
    }

    bool CContextSimulatorProxy::resetToModelMatchingAircraft(const CCallsign &callsign)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("resetToModelMatchingAircraft"), callsign);
    }

    bool CContextSimulatorProxy::requestElevationBySituation(const CAircraftSituation &situation)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("requestElevationBySituation"), situation);
    }

    CElevationPlane CContextSimulatorProxy::findClosestElevationWithinRange(const CCoordinateGeodetic &reference,
                                                                            const CLength &range) const
    {
        return m_dBusInterface->callDBusRet<swift::misc::geo::CElevationPlane>(
            QLatin1String("findClosestElevationWithinRange"), reference, range);
    }

    CStatusMessageList
    CContextSimulatorProxy::getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const
    {
        return m_dBusInterface->callDBusRet<swift::misc::CStatusMessageList>(QLatin1String("getMatchingMessages"),
                                                                             callsign);
    }

    MatchingLog CContextSimulatorProxy::isMatchingMessagesEnabled() const
    {
        return m_dBusInterface->callDBusRet<MatchingLog>(QLatin1String("isMatchingMessagesEnabled"));
    }

    void CContextSimulatorProxy::enableMatchingMessages(MatchingLog enabled)
    {
        m_dBusInterface->callDBus(QLatin1String("enableMatchingMessages"), enabled);
    }

    bool CContextSimulatorProxy::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("parseCommandLine"), commandLine, originator);
    }

    int CContextSimulatorProxy::doMatchingsAgain()
    {
        return m_dBusInterface->callDBusRet<int>(QLatin1String("doMatchingsAgain"));
    }

    bool CContextSimulatorProxy::doMatchingAgain(const CCallsign &callsign)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("doMatchingAgain"), callsign);
    }

    CMatchingStatistics CContextSimulatorProxy::getCurrentMatchingStatistics(bool missingOnly) const
    {
        return m_dBusInterface->callDBusRet<CMatchingStatistics>(QLatin1String("getCurrentMatchingStatistics"),
                                                                 missingOnly);
    }

    void CContextSimulatorProxy::setMatchingSetup(const CAircraftMatcherSetup &setup)
    {
        m_dBusInterface->callDBus(QLatin1String("setMatchingSetup"), setup);
    }

    CAircraftMatcherSetup CContextSimulatorProxy::getMatchingSetup() const
    {
        return m_dBusInterface->callDBusRet<CAircraftMatcherSetup>(QLatin1String("getMatchingSetup"));
    }

    bool CContextSimulatorProxy::testRemoteAircraft(const CSimulatedAircraft &aircraft, bool add)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("testRemoteAircraft"), aircraft, add);
    }

    bool CContextSimulatorProxy::testUpdateRemoteAircraft(const CCallsign &cs, const CAircraftSituation &situation,
                                                          const CAircraftParts &parts)
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("testUpdateRemoteAircraft"), cs, situation, parts);
    }

} // namespace swift::core::context
