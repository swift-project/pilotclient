// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/corefacadeconfig.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorplugininfolist.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/geo/elevationplane.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/aviation/airportlist.h"
#include "misc/pixmap.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;
namespace swift::misc
{
    class CGenericDBusInterface;
    namespace simulation
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
            virtual swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAvailableSimulatorPlugins
            virtual swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorSettings
            virtual swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setSimulatorSettings
            virtual bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings, const swift::misc::simulation::CSimulatorInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::startSimulatorPlugin
            virtual bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::stopSimulatorPlugin
            virtual void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::checkListeners
            virtual int checkListeners() override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorStatus
            virtual int getSimulatorStatus() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAirportsInRange
            virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSet
            virtual swift::misc::simulation::CAircraftModelList getModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::simulatorsWithInitializedModelSet
            virtual swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::verifyPrerequisites
            virtual swift::misc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetLoaderSimulator
            virtual swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setModelSetLoaderSimulator
            virtual void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetStrings
            virtual QStringList getModelSetStrings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCompleterStrings
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::removeModelsFromSet
            virtual int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isKnownModelInSet
            virtual bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetModelsStartingWith
            virtual swift::misc::simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCount
            virtual int getModelSetCount() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorInternals
            virtual swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::disableModelsForMatching
            virtual void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getDisabledModelsForMatching
            virtual swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::triggerModelSetValidation
            virtual bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isValidationInProgress
            virtual bool isValidationInProgress() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::restoreDisabledModels
            virtual void restoreDisabledModels() override;

            //! \copydoc BlackCore::Context::IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            virtual void setInterpolationAndRenderingSetupGlobal(const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            virtual swift::misc::simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(const swift::misc::simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationMessages
            virtual swift::misc::CStatusMessageList getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getTimeSynchronizationOffset
            virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::iconForModel
            virtual swift::misc::CPixmap iconForModel(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::followAircraft
            virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::recalculateAllAircraft
            virtual void recalculateAllAircraft() override;

            //! \copydoc BlackCore::Context::IContextSimulator::resetToModelMatchingAircraft
            virtual bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::requestElevationBySituation
            virtual bool requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) override;

            //! \copydoc BlackCore::Context::IContextSimulator::findClosestElevationWithinRange
            virtual swift::misc::geo::CElevationPlane findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference, const swift::misc::physical_quantities::CLength &range) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingMessages
            virtual swift::misc::CStatusMessageList getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isMatchingMessagesEnabled
            virtual swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::enableMatchingMessages
            virtual void enableMatchingMessages(swift::misc::simulation::MatchingLog enabled) override;

            //! \copydoc BlackCore::Context::IContextSimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingsAgain
            virtual int doMatchingsAgain() override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingAgain
            virtual bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getCurrentMatchingStatistics
            virtual swift::misc::simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setMatchingSetup
            virtual void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingSetup
            virtual swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::testRemoteAircraft
            virtual bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool add) override;

            //! \copydoc BlackCore::Context::IContextSimulator::testUpdateRemoteAircraft
            virtual bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs, const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CAircraftParts &parts) override;
            //! @}

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface = nullptr;

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
