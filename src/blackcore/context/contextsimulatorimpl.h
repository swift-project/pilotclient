// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/context/contextsimulator.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/simulator.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/aircraftmatcher.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/weathermanager.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/modelmatchersettings.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/interpolationrenderingsetupsettings.h"
#include "blackmisc/simulation/aircraftmodelsetprovider.h"
#include "blackmisc/simulation/backgroundvalidation.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/identifier.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/worker.h"

#include <QObject>
#include <QPair>
#include <QString>
#include <QPointer>

// clazy:excludeall=const-signal-or-slot

namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation
    {
        class CCallsign;
    }
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}

namespace BlackCore
{
    class CCoreFacade;
    class CPluginManagerSimulator;

    namespace Context
    {
        //! Network simulator concrete implementation
        class BLACKCORE_EXPORT CContextSimulator :
            public IContextSimulator,
            public BlackMisc::Simulation::IAircraftModelSetProvider,
            public BlackMisc::Simulation::CRemoteAircraftAware, // gain access to in memory remote aircraft data
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IAircraftModelSetProvider)
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
            friend class BlackCore::CCoreFacade;
            friend class IContextSimulator;

        public slots:
            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorPluginInfo
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAvailableSimulatorPlugins
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorSettings
            virtual BlackMisc::Simulation::Settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setSimulatorSettings
            virtual bool setSimulatorSettings(const BlackMisc::Simulation::Settings::CSimulatorSettings &settings, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::startSimulatorPlugin
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::stopSimulatorPlugin
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::checkListeners
            virtual int checkListeners() override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorStatus
            virtual int getSimulatorStatus() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorInternals
            virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetLoaderSimulator
            virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setModelSetLoaderSimulator
            virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::simulatorsWithInitializedModelSet
            virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::verifyPrerequisites
            virtual BlackMisc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetStrings
            virtual QStringList getModelSetStrings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCompleterStrings
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isKnownModelInSet
            virtual bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::removeModelsFromSet
            virtual int removeModelsFromSet(const BlackMisc::Simulation::CAircraftModelList &removeModels) override;

            //! \copydoc BlackCore::Context::IContextSimulator::disableModelsForMatching
            virtual void disableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &removedModels, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getDisabledModelsForMatching
            virtual BlackMisc::Simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::restoreDisabledModels
            virtual void restoreDisabledModels() override;

            //! \copydoc BlackCore::Context::IContextSimulator::isValidationInProgress
            virtual bool isValidationInProgress() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::triggerModelSetValidation
            virtual bool triggerModelSetValidation(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetModelsStartingWith
            virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            virtual BlackMisc::Simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(const BlackMisc::Simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            virtual void setInterpolationAndRenderingSetupGlobal(const BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationMessages
            virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

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

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingsAgain
            virtual int doMatchingsAgain() override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingAgain
            virtual bool doMatchingAgain(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingMessages
            virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isMatchingMessagesEnabled
            virtual BlackMisc::Simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::enableMatchingMessages
            virtual void enableMatchingMessages(BlackMisc::Simulation::MatchingLog enabled) override;

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

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSet
            virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCount
            virtual int getModelSetCount() const override;

            //! \ingroup swiftdotcommands
            //! <pre>
            //! .plugin           forwarded to plugin, see details there
            //! .driver .drv      forwarded to plugin (same as above)
            //! .ris show         show interpolation setup in console
            //! .ris debug on|off interpolation/rendering setup, debug messages
            //! .ris parts on|off interpolation/rendering setup, aircraft parts
            //! </pre>
            //! \copydoc IContextSimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            // ----------------------------- context interface -----------------------------

        public:
            //! Destructor
            virtual ~CContextSimulator() override;

            //! Gracefully shut down, e.g. for plugin unloading
            void gracefulShutdown();

            //! Access to simulator (i.e. the plugin)
            QPointer<ISimulator> simulator() const;

            //! Simulator available?
            bool hasSimulator() const { return this->simulator(); }

            //! Register dot commands
            static void registerHelp()
            {
                if (BlackMisc::CSimpleCommandParser::registered("BlackCore::CContextSimulator")) { return; }
                BlackMisc::CSimpleCommandParser::registerCommand({ ".ris", "rendering/interpolation setup (global setup)" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".ris debug on|off", "rendering/interpolation debug messages (global setup)" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".ris parts on|off", "aircraft parts (global setup)" });
            }

        protected:
            //! Constructor
            CContextSimulator(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextSimulator *registerWithDBus(BlackMisc::CDBusServer *server);

            //! Simulator plugin available?
            bool isSimulatorPluginAvailable() const;

        private:
            static constexpr int MatchingLogMaxModelSetSize = 250; //!< default value for switching matching log on
            static constexpr int MaxModelAddedFailoverTrials = 3; //!< if model cannot be added, try again max <n> times

            //  ------------ slots connected with network or other contexts ---------

            //! Remote aircraft added and ready for model matching
            //! \ingroup crosscontextfunction
            void xCtxAddedRemoteAircraftReadyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! Remove remote aircraft
            //! \ingroup crosscontextfunction
            void xCtxRemovedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! Changed remote aircraft model
            //! \ingroup crosscontextfunction
            void xCtxChangedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Changed own aircraft model
            //! \ingroup crosscontextfunction
            void xCtxChangedOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &aircraftModel, const BlackMisc::CIdentifier &originator);

            //! Enable / disable aircraft
            //! \ingroup crosscontextfunction
            void xCtxChangedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Network connection status
            //! \ingroup crosscontextfunction
            void xCtxNetworkConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \ingroup crosscontextfunction
            void xCtxUpdateSimulatorCockpitFromContext(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator);

            //! Update simulator SELCAL from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \ingroup crosscontextfunction
            void xCtxUpdateSimulatorSelcalFromContext(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator);

            //! Raw data when a new aircraft was requested, used for statistics
            //! \ingroup crosscontextfunction
            void xCtxNetworkRequestedNewAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! Text message received
            //! \ingroup crosscontextfunction
            void xCtxTextMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);
            //  ------------ slots connected with network or other contexts ---------

            //! Handle new connection status of simulator
            void onSimulatorStatusChanged(ISimulator::SimulatorStatus status);

            //! Listener reports the simulator has started
            void onSimulatorStarted(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

            //! Simulator model has been changed
            void onOwnSimulatorModelChanged(const BlackMisc::Simulation::CAircraftModel &model);

            //! Failed adding remote aircraft
            void onAddingRemoteAircraftFailed(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, bool disabled, bool requestFailover, const BlackMisc::CStatusMessage &message);

            //! Weather request was received
            void onWeatherGridReceived(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier);

            //! Relay status message to simulator under consideration of settings
            void relayStatusMessageToSimulator(const BlackMisc::CStatusMessage &message);

            //! Handle a change in enabled simulators
            void changeEnabledSimulators();

            //! Reads list of enabled simulators, starts listeners
            void restoreSimulatorPlugins();

            //! Load plugin and connect
            bool loadSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorPluginInfo);

            //! Unload plugin, if desired restart listeners
            void unloadSimulatorPlugin();

            //! Listen for single simulator
            bool listenForSimulator(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo);

            //! Listen for all simulators
            void listenForAllSimulators();

            //! Call stop() on all loaded listeners
            void stopSimulatorListeners();

            //! Add to message list for matching
            void addMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CStatusMessageList &messages);

            //! Clear matching messages
            void clearMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign);

            //! Reverse lookup of a model
            BlackMisc::Simulation::CAircraftModel reverseLookupModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Load the last know model set
            void initByLastUsedModelSet();

            //! Init and set validator
            void setValidator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            QPair<BlackMisc::Simulation::CSimulatorPluginInfo, QPointer<ISimulator>> m_simulatorPlugin; //!< Currently loaded simulator plugin
            QMap<BlackMisc::Aviation::CCallsign, BlackMisc::CStatusMessageList> m_matchingMessages; //!< all matching log messages per callsign
            QMap<BlackMisc::Aviation::CCallsign, int> m_failoverAddingCounts;
            CPluginManagerSimulator *m_plugins = nullptr; //!< plugin manager
            BlackMisc::CRegularThread m_listenersThread; //!< waiting for plugin
            CWeatherManager m_weatherManager { this }; //!< weather management
            CAircraftMatcher m_aircraftMatcher { this }; //!< model matcher

            bool m_wasSimulating = false;
            bool m_initallyAddAircraft = false;
            bool m_isWeatherActivated = false; // used to activate after plugin is loaded
            BlackMisc::Simulation::MatchingLog m_logMatchingMessages = BlackMisc::Simulation::MatchingLogSimplified;

            QString m_networkSessionId; //!< Network session of CServer::getServerSessionId, if not connected empty (for statistics, ..)
            BlackMisc::Simulation::CBackgroundValidation *m_validator = nullptr;

            // settings
            BlackMisc::CSettingReadOnly<Application::TEnabledSimulators> m_enabledSimulators { this, &CContextSimulator::changeEnabledSimulators };
            BlackMisc::CSetting<BlackMisc::Simulation::Settings::TModelMatching> m_matchingSettings { this }; //!< matching settings (all simulators)
            BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelection> m_modelSetSimulator { this }; //!< current simulator used for model set
            BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_multiSimulatorSettings { this }; //!< individual for simulator, for validation of models
            BlackMisc::CSetting<BlackMisc::Simulation::Settings::TInterpolationAndRenderingSetupGlobal> m_renderSettings { this }; //!< rendering/interpolation settings (all simulators)
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TSimulatorMessages> m_messageSettings { this }; //!< settings for messages (all simulators)
        };
    } // namespace
} // namespace

#endif // guard
