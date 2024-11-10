// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H
#define SWIFT_CORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H

#include "core/swiftcoreexport.h"
#include "core/context/contextsimulator.h"
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
        virtual swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatorPluginInfo();
        }

        //! \copydoc IContextSimulator::getAvailableSimulatorPlugins
        virtual swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatorPluginInfoList();
        }

        //! \copydoc IContextSimulator::getSimulatorSettings
        virtual swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::settings::CSimulatorSettings();
        }

        //! \copydoc IContextSimulator::setSimulatorSettings
        virtual bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings, const swift::misc::simulation::CSimulatorInfo &simulatorInfo) override
        {
            Q_UNUSED(settings)
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::startSimulatorPlugin
        virtual bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override
        {
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::checkListeners
        virtual int checkListeners() override
        {
            return 0;
        }

        //! \copydoc IContextSimulator::getSimulatorStatus
        virtual int getSimulatorStatus() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextSimulator::stopSimulatorPlugin
        virtual void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override
        {
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::getAirportsInRange
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override
        {
            Q_UNUSED(recalculatePosition)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::aviation::CAirportList();
        }

        //! \copydoc IContextSimulator::getModelSet
        virtual swift::misc::simulation::CAircraftModelList getModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CAircraftModelList();
        }

        //! \copydoc IContextSimulator::simulatorsWithInitializedModelSet
        virtual swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatorInfo();
        }

        //! \copydoc IContextSimulator::verifyPrerequisites
        virtual swift::misc::CStatusMessageList verifyPrerequisites() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::getModelSetLoaderSimulator
        virtual swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatorInfo();
        }

        //! \copydoc IContextSimulator::setModelSetLoaderSimulator
        virtual void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override
        {
            Q_UNUSED(simulator);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::getModelSetStrings
        virtual QStringList getModelSetStrings() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return QStringList();
        }

        //! \copydoc IContextSimulator::getModelSetCompleterStrings
        virtual QStringList getModelSetCompleterStrings(bool sorted) const override
        {
            Q_UNUSED(sorted);
            logEmptyContextWarning(Q_FUNC_INFO);
            return QStringList();
        }

        //! \copydoc IContextSimulator::removeModelsFromSet
        virtual int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override
        {
            Q_UNUSED(removeModels);
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc CAircraftMatcher::disableModelsForMatching
        virtual void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels, bool incremental) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(removedModels);
            Q_UNUSED(incremental);
        }

        //! \copydoc CAircraftMatcher::getDisabledModelsForMatching
        virtual swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CAircraftModelList();
        }

        //! \copydoc CAircraftMatcher::restoreDisabledModels
        virtual void restoreDisabledModels() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc swift::misc::simulation::CBackgroundValidation::triggerValidation
        virtual bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(simulator);
            return false;
        }

        //! \copydoc swift::misc::simulation::CBackgroundValidation::isValidating
        virtual bool isValidationInProgress() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::isKnownModelInSet
        virtual bool isKnownModelInSet(const QString &modelstring) const override
        {
            Q_UNUSED(modelstring);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getModelSetModelsStartingWith
        virtual swift::misc::simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override
        {
            Q_UNUSED(modelString);
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CAircraftModelList();
        }

        //! \copydoc IContextSimulator::getModelSetCount
        virtual int getModelSetCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextSimulator::getSimulatorInternals
        virtual swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CSimulatorInternals();
        }

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) override
        {
            Q_UNUSED(enable)
            Q_UNUSED(offset)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc ISimulator::getInterpolationSetupGlobal
        virtual swift::misc::simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CInterpolationAndRenderingSetupGlobal();
        }

        //! \copydoc ISimulator::getInterpolationSetupsPerCallsign
        virtual swift::misc::simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CInterpolationSetupList();
        }

        //! \copydoc ISimulator::getInterpolationSetupPerCallsignOrDefault
        virtual swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign();
        }

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        virtual void setInterpolationAndRenderingSetupGlobal(const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupsPerCallsign
        virtual bool setInterpolationAndRenderingSetupsPerCallsign(const swift::misc::simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override
        {
            Q_UNUSED(setups)
            Q_UNUSED(ignoreSameAsGlobal)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getInterpolationMessages
        virtual swift::misc::CStatusMessageList getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::physical_quantities::CTime();
        }

        //! \copydoc IContextSimulator::iconForModel
        virtual swift::misc::CPixmap iconForModel(const QString &modelString) const override
        {
            Q_UNUSED(modelString)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::CPixmap();
        }

        //! \copydoc IContextSimulator::followAircraft
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::recalculateAllAircraft
        virtual void recalculateAllAircraft() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::resetToModelMatchingAircraft
        virtual bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextSimulator::doMatchingsAgain
        virtual int doMatchingsAgain() override
        {
            return 0;
        }

        //! \copydoc IContextSimulator::doMatchingAgain
        virtual bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextSimulator::requestElevationBySituation
        virtual bool requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) override
        {
            Q_UNUSED(situation)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::findClosestElevationWithinRange
        virtual swift::misc::geo::CElevationPlane findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference, const swift::misc::physical_quantities::CLength &range) const override
        {
            Q_UNUSED(reference)
            Q_UNUSED(range)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::geo::CElevationPlane::null();
        }

        //! \copydoc IContextSimulator::getMatchingMessages
        virtual swift::misc::CStatusMessageList getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::enableMatchingMessages
        virtual void enableMatchingMessages(swift::misc::simulation::MatchingLog enable) override
        {
            Q_UNUSED(enable)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::isMatchingMessagesEnabled
        virtual swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::MatchingLogNothing;
        }

        //! \copydoc IContextSimulator::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getCurrentMatchingStatistics
        virtual swift::misc::simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override
        {
            Q_UNUSED(missingOnly)
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CMatchingStatistics();
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        virtual void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::testRemoteAircraft
        virtual bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool add) override
        {
            Q_UNUSED(aircraft)
            Q_UNUSED(add)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::testUpdateRemoteAircraft
        virtual bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs, const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CAircraftParts &parts) override
        {
            Q_UNUSED(cs)
            Q_UNUSED(situation)
            Q_UNUSED(parts)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        virtual swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return swift::misc::simulation::CAircraftMatcherSetup();
        }
    };
} // namespace

#endif // guard
