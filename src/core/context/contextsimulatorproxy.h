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
            virtual ~CContextSimulatorProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorPluginInfo
            virtual swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc swift::core::context::IContextSimulator::getAvailableSimulatorPlugins
            virtual swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorSettings
            virtual swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc swift::core::context::IContextSimulator::setSimulatorSettings
            virtual bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings,
                                              const swift::misc::simulation::CSimulatorInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::startSimulatorPlugin
            virtual bool
            startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::stopSimulatorPlugin
            virtual void
            stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc swift::core::context::IContextSimulator::checkListeners
            virtual int checkListeners() override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorStatus
            virtual ISimulator::SimulatorStatus getSimulatorStatus() const override;

            //! \copydoc swift::core::context::IContextSimulator::getAirportsInRange
            virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSet
            virtual swift::misc::simulation::CAircraftModelList getModelSet() const override;

            //! \copydoc swift::core::context::IContextSimulator::simulatorsWithInitializedModelSet
            virtual swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc swift::core::context::IContextSimulator::verifyPrerequisites
            virtual swift::misc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetLoaderSimulator
            virtual swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc swift::core::context::IContextSimulator::setModelSetLoaderSimulator
            virtual void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetStrings
            virtual QStringList getModelSetStrings() const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetCompleterStrings
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc swift::core::context::IContextSimulator::removeModelsFromSet
            virtual int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override;

            //! \copydoc swift::core::context::IContextSimulator::isKnownModelInSet
            virtual bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetModelsStartingWith
            virtual swift::misc::simulation::CAircraftModelList
            getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc swift::core::context::IContextSimulator::getModelSetCount
            virtual int getModelSetCount() const override;

            //! \copydoc swift::core::context::IContextSimulator::getSimulatorInternals
            virtual swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc swift::core::context::IContextSimulator::disableModelsForMatching
            virtual void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels,
                                                  bool incremental) override;

            //! \copydoc swift::core::context::IContextSimulator::getDisabledModelsForMatching
            virtual swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc swift::core::context::IContextSimulator::triggerModelSetValidation
            virtual bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc swift::core::context::IContextSimulator::isValidationInProgress
            virtual bool isValidationInProgress() const override;

            //! \copydoc swift::core::context::IContextSimulator::restoreDisabledModels
            virtual void restoreDisabledModels() override;

            //! \copydoc swift::core::context::IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable,
                                                const swift::misc::physical_quantities::CTime &offset) override;

            //! \copydoc swift::core::context::IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupGlobal
            getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc swift::core::context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            virtual void setInterpolationAndRenderingSetupGlobal(
                const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            virtual swift::misc::simulation::CInterpolationSetupList
            getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign
            getInterpolationAndRenderingSetupPerCallsignOrDefault(
                const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(
                const swift::misc::simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc swift::core::context::IContextSimulator::getInterpolationMessages
            virtual swift::misc::CStatusMessageList
            getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::getTimeSynchronizationOffset
            virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc swift::core::context::IContextSimulator::followAircraft
            virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::recalculateAllAircraft
            virtual void recalculateAllAircraft() override;

            //! \copydoc swift::core::context::IContextSimulator::resetToModelMatchingAircraft
            virtual bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::requestElevationBySituation
            virtual bool
            requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) override;

            //! \copydoc swift::core::context::IContextSimulator::findClosestElevationWithinRange
            virtual swift::misc::geo::CElevationPlane
            findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference,
                                            const swift::misc::physical_quantities::CLength &range) const override;

            //! \copydoc swift::core::context::IContextSimulator::getMatchingMessages
            virtual swift::misc::CStatusMessageList
            getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextSimulator::isMatchingMessagesEnabled
            virtual swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc swift::core::context::IContextSimulator::enableMatchingMessages
            virtual void enableMatchingMessages(swift::misc::simulation::MatchingLog enabled) override;

            //! \copydoc swift::core::context::IContextSimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextSimulator::doMatchingsAgain
            virtual int doMatchingsAgain() override;

            //! \copydoc swift::core::context::IContextSimulator::doMatchingAgain
            virtual bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextSimulator::getCurrentMatchingStatistics
            virtual swift::misc::simulation::CMatchingStatistics
            getCurrentMatchingStatistics(bool missingOnly) const override;

            //! \copydoc swift::core::context::IContextSimulator::setMatchingSetup
            virtual void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) override;

            //! \copydoc swift::core::context::IContextSimulator::getMatchingSetup
            virtual swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const override;

            //! \copydoc swift::core::context::IContextSimulator::testRemoteAircraft
            virtual bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                            bool add) override;

            //! \copydoc swift::core::context::IContextSimulator::testUpdateRemoteAircraft
            virtual bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs,
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
