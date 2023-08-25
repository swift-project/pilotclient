// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H
#define BLACKCORE_CONTEXT_CONTEXTSIMULATOR_EMPTY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/logmessage.h"

// clazy:excludeall=const-signal-or-slot

namespace BlackCore::Context
{
    //! Empty context, used during shutdown/initialization
    class BLACKCORE_EXPORT CContextSimulatorEmpty : public IContextSimulator
    {
        Q_OBJECT

    public:
        //! Constructor
        CContextSimulatorEmpty(CCoreFacade *runtime) : IContextSimulator(CCoreFacadeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextSimulator::getSimulatorPluginInfo
        virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatorPluginInfo();
        }

        //! \copydoc IContextSimulator::getAvailableSimulatorPlugins
        virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatorPluginInfoList();
        }

        //! \copydoc IContextSimulator::getSimulatorSettings
        virtual BlackMisc::Simulation::Settings::CSimulatorSettings getSimulatorSettings() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::Settings::CSimulatorSettings();
        }

        //! \copydoc IContextSimulator::setSimulatorSettings
        virtual bool setSimulatorSettings(const BlackMisc::Simulation::Settings::CSimulatorSettings &settings, const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo) override
        {
            Q_UNUSED(settings)
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::startSimulatorPlugin
        virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override
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
        virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override
        {
            Q_UNUSED(simulatorInfo)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::getAirportsInRange
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override
        {
            Q_UNUSED(recalculatePosition)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CAirportList();
        }

        //! \copydoc IContextSimulator::getModelSet
        virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CAircraftModelList();
        }

        //! \copydoc IContextSimulator::simulatorsWithInitializedModelSet
        virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatorInfo();
        }

        //! \copydoc IContextSimulator::verifyPrerequisites
        virtual BlackMisc::CStatusMessageList verifyPrerequisites() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::getModelSetLoaderSimulator
        virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatorInfo();
        }

        //! \copydoc IContextSimulator::setModelSetLoaderSimulator
        virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override
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
        virtual int removeModelsFromSet(const BlackMisc::Simulation::CAircraftModelList &removeModels) override
        {
            Q_UNUSED(removeModels);
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc CAircraftMatcher::disableModelsForMatching
        virtual void disableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &removedModels, bool incremental) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(removedModels);
            Q_UNUSED(incremental);
        }

        //! \copydoc CAircraftMatcher::getDisabledModelsForMatching
        virtual BlackMisc::Simulation::CAircraftModelList getDisabledModelsForMatching() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CAircraftModelList();
        }

        //! \copydoc CAircraftMatcher::restoreDisabledModels
        virtual void restoreDisabledModels() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc BlackMisc::Simulation::CBackgroundValidation::triggerValidation
        virtual bool triggerModelSetValidation(const BlackMisc::Simulation::CSimulatorInfo &simulator) override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            Q_UNUSED(simulator);
            return false;
        }

        //! \copydoc BlackMisc::Simulation::CBackgroundValidation::isValidating
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
        virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override
        {
            Q_UNUSED(modelString);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CAircraftModelList();
        }

        //! \copydoc IContextSimulator::getModelSetCount
        virtual int getModelSetCount() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc IContextSimulator::getSimulatorInternals
        virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CSimulatorInternals();
        }

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override
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
        virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal();
        }

        //! \copydoc ISimulator::getInterpolationSetupsPerCallsign
        virtual BlackMisc::Simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CInterpolationSetupList();
        }

        //! \copydoc ISimulator::getInterpolationSetupPerCallsignOrDefault
        virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign();
        }

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        virtual void setInterpolationAndRenderingSetupGlobal(const BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::setInterpolationSetupsPerCallsign
        virtual bool setInterpolationAndRenderingSetupsPerCallsign(const BlackMisc::Simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override
        {
            Q_UNUSED(setups)
            Q_UNUSED(ignoreSameAsGlobal)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getInterpolationMessages
        virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::PhysicalQuantities::CTime();
        }

        //! \copydoc IContextSimulator::iconForModel
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override
        {
            Q_UNUSED(modelString)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::CPixmap();
        }

        //! \copydoc IContextSimulator::highlightAircraft
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override
        {
            Q_UNUSED(aircraftToHighlight)
            Q_UNUSED(enableHighlight)
            Q_UNUSED(displayTime)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::followAircraft
        virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) override
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
        virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) override
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
        virtual bool doMatchingAgain(const BlackMisc::Aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign)
            return false;
        }

        //! \copydoc IContextSimulator::isWeatherActivated
        virtual bool isWeatherActivated() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::setWeatherActivated
        virtual void setWeatherActivated(bool activated) override
        {
            Q_UNUSED(activated)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::requestWeatherGrid
        virtual void requestWeatherGrid(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::CIdentifier &identifier) override
        {
            Q_UNUSED(position)
            Q_UNUSED(identifier)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::requestWeatherGrid
        virtual bool requestElevationBySituation(const BlackMisc::Aviation::CAircraftSituation &situation) override
        {
            Q_UNUSED(situation)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::findClosestElevationWithinRange
        virtual BlackMisc::Geo::CElevationPlane findClosestElevationWithinRange(const BlackMisc::Geo::CCoordinateGeodetic &reference, const BlackMisc::PhysicalQuantities::CLength &range) const override
        {
            Q_UNUSED(reference)
            Q_UNUSED(range)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Geo::CElevationPlane::null();
        }

        //! \copydoc IContextSimulator::getMatchingMessages
        virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::CStatusMessageList();
        }

        //! \copydoc IContextSimulator::enableMatchingMessages
        virtual void enableMatchingMessages(BlackMisc::Simulation::MatchingLog enable) override
        {
            Q_UNUSED(enable)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::isMatchingMessagesEnabled
        virtual BlackMisc::Simulation::MatchingLog isMatchingMessagesEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::MatchingLogNothing;
        }

        //! \copydoc IContextSimulator::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine)
            Q_UNUSED(originator)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::getCurrentMatchingStatistics
        virtual BlackMisc::Simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override
        {
            Q_UNUSED(missingOnly)
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CMatchingStatistics();
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        virtual void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup) override
        {
            Q_UNUSED(setup)
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextSimulator::testRemoteAircraft
        virtual bool testRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool add) override
        {
            Q_UNUSED(aircraft)
            Q_UNUSED(add)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::testUpdateRemoteAircraft
        virtual bool testUpdateRemoteAircraft(const BlackMisc::Aviation::CCallsign &cs, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override
        {
            Q_UNUSED(cs)
            Q_UNUSED(situation)
            Q_UNUSED(parts)
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextSimulator::setMatchingSetup
        virtual BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Simulation::CAircraftMatcherSetup();
        }

        //! \copydoc IContextSimulator::copyFsxTerrainProbe
        virtual BlackMisc::CStatusMessageList copyFsxTerrainProbe(const BlackMisc::Simulation::CSimulatorInfo &simulator) override
        {
            Q_UNUSED(simulator)
            return BlackMisc::CStatusMessageList();
        }
    };
} // namespace

#endif // guard
