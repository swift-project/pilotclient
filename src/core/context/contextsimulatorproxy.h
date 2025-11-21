// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXTSIMULATOR_PROXY_H
#define SWIFT_CORE_CONTEXTSIMULATOR_PROXY_H

#include <QString>

#include "core/context/contextsimulator.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/airportlist.h"
#include "misc/geo/elevationplane.h"
#include "misc/pixmap.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/simulation/simulatorplugininfolist.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;
namespace swift::misc
{
    class CGenericDBusInterface;
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! DBus proxy for Simulator Context
        //! \ingroup dbus
        class SWIFT_CORE_EXPORT CContextSimulatorProxy : public IContextSimulator
        {
            Q_OBJECT
            friend class IContextSimulator;

        public:
            //! Destructor
            ~CContextSimulatorProxy() override = default;

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorPluginInfo
            swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc swift::core::context::IContextSimulator::getAvailableSimulatorPlugins
            swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorSettings
            swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc swift::core::context::IContextSimulator::setSimulatorSettings
            bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings,
                                      const swift::misc::simulation::CSimulatorInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::startSimulatorPlugin
            bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::stopSimulatorPlugin
            void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::checkListeners
            int checkListeners() override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorStatus
            ISimulator::SimulatorStatus getSimulatorStatus() const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSet
            swift::misc::simulation::CAircraftModelList getModelSet() const override;

            //! \copydoc swift::core::context::IContextSimulator::simulatorsWithInitializedModelSet
            swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc swift::core::context::IContextSimulator::verifyPrerequisites
            swift::misc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetLoaderSimulator
            swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc swift::core::context::IContextSimulator::setModelSetLoaderSimulator
            void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetStrings
            QStringList getModelSetStrings() const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetCompleterStrings
            QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc swift::core::context::IContextSimulator::removeModelsFromSet
            int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override;

            //! \copydoc swift::core::context::IContextSimulator::isKnownModelInSet
            bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetModelsStartingWith
            swift::misc::simulation::CAircraftModelList
            getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetCount
            int getModelSetCount() const override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorInternals
            swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc swift::core::context::IContextSimulator::disableModelsForMatching
            void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels,
                                          bool incremental) override;

            //! \copydoc swift::core::context::IContextSimulator::getDisabledModelsForMatching
            swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc swift::core::context::IContextSimulator::triggerModelSetValidation
            bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc swift::core::context::IContextSimulator::isValidationInProgress
            bool isValidationInProgress() const override;

            //! \copydoc swift::core::context::IContextSimulator::restoreDisabledModels
            void restoreDisabledModels() override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            swift::misc::simulation::CInterpolationAndRenderingSetupGlobal
            getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc swift::core::context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            void setInterpolationAndRenderingSetupGlobal(
                const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            swift::misc::simulation::CInterpolationSetupList
            getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign
            getInterpolationAndRenderingSetupPerCallsignOrDefault(
                const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            bool setInterpolationAndRenderingSetupsPerCallsign(
                const swift::misc::simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationMessages
            swift::misc::CStatusMessageList
            getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::followAircraft
            bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::recalculateAllAircraft
            void recalculateAllAircraft() override;

            //! \copydoc swift::core::context::IContextSimulator::resetToModelMatchingAircraft
            bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::requestElevationBySituation
            bool requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) override;

            //! \copydoc swift::core::context::IContextSimulator::findClosestElevationWithinRange
            swift::misc::geo::CElevationPlane
            findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference,
                                            const swift::misc::physical_quantities::CLength &range) const override;

            //! \copydoc swift::core::context::IContextSimulator::getMatchingMessages
            swift::misc::CStatusMessageList
            getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::isMatchingMessagesEnabled
            swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc swift::core::context::IContextSimulator::enableMatchingMessages
            void enableMatchingMessages(swift::misc::simulation::MatchingLog enabled) override;

            //! \copydoc swift::core::context::IContextSimulator::parseCommandLine
            bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextSimulator::doMatchingsAgain
            int doMatchingsAgain() override;

            //! \copydoc swift::core::context::IContextSimulator::doMatchingAgain
            bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::getCurrentMatchingStatistics
            swift::misc::simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override;

            //! \copydoc swift::core::context::IContextSimulator::setMatchingSetup
            void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) override;

            //! \copydoc swift::core::context::IContextSimulator::getMatchingSetup
            swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const override;

            //! \copydoc swift::core::context::IContextSimulator::testRemoteAircraft
            bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool add) override;

            //! \copydoc swift::core::context::IContextSimulator::testUpdateRemoteAircraft
            bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs,
                                          const swift::misc::aviation::CAircraftSituation &situation,
                                          const swift::misc::aviation::CAircraftParts &parts) override;
            //! @}

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface = nullptr;

            //! Relay connection signals to local signals
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextSimulatorProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
                : IContextSimulator(mode, runtime)
            {}

            //! DBus version constructor
            CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection,
                                   CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // namespace context
} // namespace swift::core
#endif // SWIFT_CORE_CONTEXTSIMULATOR_PROXY_H
