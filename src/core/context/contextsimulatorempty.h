// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H
#define SWIFT_CORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H

#include "core/context/contextsimulator.h"
#include "core/swiftcoreexport.h"
#include "misc/logmessage.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::core::context
{
    //! Empty context, used during shutdown/initialization
    class SWIFT_CORE_EXPORT CContextSimulatorEmpty : public IContextSimulator
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextSimulatorEmpty(CCoreFacade *runtime) : IContextSimulator(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextSimulator::getSimulatorPluginInfo
        swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::getAvailableSimulatorPlugins
        swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::getSimulatorSettings
        swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::setSimulatorSettings
        bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings,
                                  const swift::misc::simulation::CSimulatorInfo &simulatorInfo) override
        {
            Q_UNUSED(settings)
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::startSimulatorPlugin
        bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override
        {
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::checkListeners
        int checkListeners() override { return 0; }

        //! \copydoc IContextSimulator::getSimulatorStatus
        ISimulator::SimulatorStatus getSimulatorStatus() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return ISimulator::Unspecified;
        }

        //! \copydoc IContextSimulator::stopSimulatorPlugin
        void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override
        {
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::getModelSet
        swift::misc::simulation::CAircraftModelList getModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::simulatorsWithInitializedModelSet
        swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::verifyPrerequisites
        swift::misc::CStatusMessageList verifyPrerequisites() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::getModelSetLoaderSimulator
        swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::setModelSetLoaderSimulator
        void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override
        {
            Q_UNUSED(simulator);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::getModelSetStrings
        QStringList getModelSetStrings() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::getModelSetCompleterStrings
        QStringList getModelSetCompleterStrings(bool sorted) const override
        {
            Q_UNUSED(sorted);
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::removeModelsFromSet
        int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override
        {
            Q_UNUSED(removeModels);
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc CAircraftMatcher::disableModelsForMatching
        void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels,
                                      bool incremental) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(removedModels);
            Q_UNUSED(incremental);
        }

        //! \copydoc CAircraftMatcher::getDisabledModelsForMatching
        swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc CAircraftMatcher::restoreDisabledModels
        void restoreDisabledModels() override { logEmptyContextWarning(Q_FUNC_INFO); }

        //! \copydoc swift::misc::simulation::CBackgroundValidation::triggerValidation
        bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(simulator);
            return false;
        }

        //! \copydoc swift::misc::simulation::CBackgroundValidation::isValidating
        bool isValidationInProgress() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::isKnownModelInSet
        bool isKnownModelInSet(const QString &modelstring) const override
        {
            Q_UNUSED(modelstring);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getModelSetModelsStartingWith
        swift::misc::simulation::CAircraftModelList
        getModelSetModelsStartingWith(const QString &modelString) const override
        {
            Q_UNUSED(modelString);
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::getModelSetCount
        int getModelSetCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextSimulator::getSimulatorInternals
        swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc ISimulator::getInterpolationSetupGlobal
        swift::misc::simulation::CInterpolationAndRenderingSetupGlobal
        getInterpolationAndRenderingSetupGlobal() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc ISimulator::getInterpolationSetupsPerCallsign
        swift::misc::simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc ISimulator::getInterpolationSetupPerCallsignOrDefault
        swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign
        getInterpolationAndRenderingSetupPerCallsignOrDefault(
            const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        void setInterpolationAndRenderingSetupGlobal(
            const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupsPerCallsign
        bool
        setInterpolationAndRenderingSetupsPerCallsign(const swift::misc::simulation::CInterpolationSetupList &setups,
                                                      bool ignoreSameAsGlobal) override
        {
            Q_UNUSED(setups)
            Q_UNUSED(ignoreSameAsGlobal)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getInterpolationMessages
        swift::misc::CStatusMessageList
        getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::followAircraft
        bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::recalculateAllAircraft
        void recalculateAllAircraft() override { logEmptyContextWarning(Q_FUNC_INFO); }

        //! \copydoc IContextSimulator::resetToModelMatchingAircraft
        bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextSimulator::doMatchingsAgain
        int doMatchingsAgain() override { return 0; }

        //! \copydoc IContextSimulator::doMatchingAgain
        bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextSimulator::requestElevationBySituation
        bool requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) override
        {
            Q_UNUSED(situation)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::findClosestElevationWithinRange
        swift::misc::geo::CElevationPlane
        findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference,
                                        const swift::misc::physical_quantities::CLength &range) const override
        {
            Q_UNUSED(reference)
            Q_UNUSED(range)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::geo::CElevationPlane::null();
        }

        //! \copydoc IContextSimulator::getMatchingMessages
        swift::misc::CStatusMessageList
        getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::enableMatchingMessages
        void enableMatchingMessages(swift::misc::simulation::MatchingLog enable) override
        {
            Q_UNUSED(enable)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::isMatchingMessagesEnabled
        swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::MatchingLogNothing;
        }

        //! \copydoc IContextSimulator::parseCommandLine
        bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getCurrentMatchingStatistics
        swift::misc::simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override
        {
            Q_UNUSED(missingOnly)
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::testRemoteAircraft
        bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool add) override
        {
            Q_UNUSED(aircraft)
            Q_UNUSED(add)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::testUpdateRemoteAircraft
        bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs,
                                      const swift::misc::aviation::CAircraftSituation &situation,
                                      const swift::misc::aviation::CAircraftParts &parts) override
        {
            Q_UNUSED(cs)
            Q_UNUSED(situation)
            Q_UNUSED(parts)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return {};
        }
    };
} // namespace swift::core::context

#endif // SWIFT_CORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H
