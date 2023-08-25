// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/pixmap.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;
namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! DBus proxy for Simulator Context
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextSimulatorProxy : public IContextSimulator
        {
            Q_OBJECT
            friend class IContextSimulator;

        public:
            //! Destructor
            virtual ~CContextSimulatorProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorPluginInfo
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAvailableSimulatorPlugins
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorSettings
            virtual BlackMisc::Simulation::Settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setSimulatorSettings
            virtual bool setSimulatorSettings(const BlackMisc::Simulation::Settings::CSimulatorSettings &settings, const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::startSimulatorPlugin
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::stopSimulatorPlugin
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::checkListeners
            virtual int checkListeners() override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorStatus
            virtual int getSimulatorStatus() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSet
            virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::simulatorsWithInitializedModelSet
            virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::verifyPrerequisites
            virtual BlackMisc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetLoaderSimulator
            virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setModelSetLoaderSimulator
            virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetStrings
            virtual QStringList getModelSetStrings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCompleterStrings
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::removeModelsFromSet
            virtual int removeModelsFromSet(const BlackMisc::Simulation::CAircraftModelList &removeModels) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isKnownModelInSet
            virtual bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetModelsStartingWith
            virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCount
            virtual int getModelSetCount() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorInternals
            virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::disableModelsForMatching
            virtual void disableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &removedModels, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getDisabledModelsForMatching
            virtual BlackMisc::Simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::triggerModelSetValidation
            virtual bool triggerModelSetValidation(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isValidationInProgress
            virtual bool isValidationInProgress() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::restoreDisabledModels
            virtual void restoreDisabledModels() override;

            //! \copydoc BlackCore::Context::IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            virtual void setInterpolationAndRenderingSetupGlobal(const BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            virtual BlackMisc::Simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(const BlackMisc::Simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationMessages
            virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::highlightAircraft
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

            //! \copydoc BlackCore::Context::IContextSimulator::followAircraft
            virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::recalculateAllAircraft
            virtual void recalculateAllAircraft() override;

            //! \copydoc BlackCore::Context::IContextSimulator::resetToModelMatchingAircraft
            virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isWeatherActivated
            virtual bool isWeatherActivated() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setWeatherActivated
            virtual void setWeatherActivated(bool activated) override;

            //! \copydoc BlackCore::Context::IContextSimulator::requestWeatherGrid
            virtual void requestWeatherGrid(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::IContextSimulator::requestElevationBySituation
            virtual bool requestElevationBySituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc BlackCore::Context::IContextSimulator::findClosestElevationWithinRange
            virtual BlackMisc::Geo::CElevationPlane findClosestElevationWithinRange(const BlackMisc::Geo::CCoordinateGeodetic &reference, const BlackMisc::PhysicalQuantities::CLength &range) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingMessages
            virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isMatchingMessagesEnabled
            virtual BlackMisc::Simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::enableMatchingMessages
            virtual void enableMatchingMessages(BlackMisc::Simulation::MatchingLog enabled) override;

            //! \copydoc BlackCore::Context::IContextSimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingsAgain
            virtual int doMatchingsAgain() override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingAgain
            virtual bool doMatchingAgain(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getCurrentMatchingStatistics
            virtual BlackMisc::Simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setMatchingSetup
            virtual void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingSetup
            virtual BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::copyFsxTerrainProbe
            virtual BlackMisc::CStatusMessageList copyFsxTerrainProbe(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::testRemoteAircraft
            virtual bool testRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool add) override;

            //! \copydoc BlackCore::Context::IContextSimulator::testUpdateRemoteAircraft
            virtual bool testUpdateRemoteAircraft(const BlackMisc::Aviation::CCallsign &cs, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override;
            //! @}

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface = nullptr;

            //! Relay connection signals to local signals
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextSimulatorProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextSimulator(mode, runtime) {}

            //! DBus version constructor
            CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard
