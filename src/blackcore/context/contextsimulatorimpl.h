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
#include "misc/network/connectionstatus.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/settings/modelmatchersettings.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/settings/interpolationrenderingsetupsettings.h"
#include "misc/simulation/aircraftmodelsetprovider.h"
#include "misc/simulation/backgroundvalidation.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulatorplugininfolist.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/aviation/airportlist.h"
#include "misc/network/textmessagelist.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/identifier.h"
#include "misc/pixmap.h"
#include "misc/settingscache.h"
#include "misc/worker.h"

#include <QObject>
#include <QPair>
#include <QString>
#include <QPointer>

// clazy:excludeall=const-signal-or-slot

namespace swift::misc
{
    class CDBusServer;
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
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
            public swift::misc::simulation::IAircraftModelSetProvider,
            public swift::misc::simulation::CRemoteAircraftAware, // gain access to in memory remote aircraft data
            public swift::misc::CIdentifiable
        {
            Q_OBJECT
            Q_INTERFACES(swift::misc::simulation::IAircraftModelSetProvider)
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
            friend class BlackCore::CCoreFacade;
            friend class IContextSimulator;

        public slots:
            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorPluginInfo
            virtual swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAvailableSimulatorPlugins
            virtual swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorSettings
            virtual swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setSimulatorSettings
            virtual bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings, const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::startSimulatorPlugin
            virtual bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::stopSimulatorPlugin
            virtual void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc BlackCore::Context::IContextSimulator::checkListeners
            virtual int checkListeners() override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorStatus
            virtual int getSimulatorStatus() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getSimulatorInternals
            virtual swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getAirportsInRange
            virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetLoaderSimulator
            virtual swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setModelSetLoaderSimulator
            virtual void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::simulatorsWithInitializedModelSet
            virtual swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::verifyPrerequisites
            virtual swift::misc::CStatusMessageList verifyPrerequisites() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetStrings
            virtual QStringList getModelSetStrings() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetCompleterStrings
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isKnownModelInSet
            virtual bool isKnownModelInSet(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::removeModelsFromSet
            virtual int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) override;

            //! \copydoc BlackCore::Context::IContextSimulator::disableModelsForMatching
            virtual void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels, bool incremental) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getDisabledModelsForMatching
            virtual swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::restoreDisabledModels
            virtual void restoreDisabledModels() override;

            //! \copydoc BlackCore::Context::IContextSimulator::isValidationInProgress
            virtual bool isValidationInProgress() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::triggerModelSetValidation
            virtual bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSetModelsStartingWith
            virtual swift::misc::simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getTimeSynchronizationOffset
            virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) override;

            //! \copydoc BlackCore::Context::IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupGlobal
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupsPerCallsign
            virtual swift::misc::simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault
            virtual swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupsPerCallsign
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(const swift::misc::simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;

            //! \copydoc BlackCore::Context::IContextSimulator::setInterpolationAndRenderingSetupGlobal
            virtual void setInterpolationAndRenderingSetupGlobal(const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getInterpolationMessages
            virtual swift::misc::CStatusMessageList getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;

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

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingsAgain
            virtual int doMatchingsAgain() override;

            //! \copydoc BlackCore::Context::IContextSimulator::doMatchingAgain
            virtual bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextSimulator::getMatchingMessages
            virtual swift::misc::CStatusMessageList getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc BlackCore::Context::IContextSimulator::isMatchingMessagesEnabled
            virtual swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const override;

            //! \copydoc BlackCore::Context::IContextSimulator::enableMatchingMessages
            virtual void enableMatchingMessages(swift::misc::simulation::MatchingLog enabled) override;

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

            //! \copydoc BlackCore::Context::IContextSimulator::getModelSet
            virtual swift::misc::simulation::CAircraftModelList getModelSet() const override;

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
            virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;
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
                if (swift::misc::CSimpleCommandParser::registered("BlackCore::CContextSimulator")) { return; }
                swift::misc::CSimpleCommandParser::registerCommand({ ".ris", "rendering/interpolation setup (global setup)" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".ris debug on|off", "rendering/interpolation debug messages (global setup)" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".ris parts on|off", "aircraft parts (global setup)" });
            }

        protected:
            //! Constructor
            CContextSimulator(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextSimulator *registerWithDBus(swift::misc::CDBusServer *server);

            //! Simulator plugin available?
            bool isSimulatorPluginAvailable() const;

        private:
            static constexpr int MatchingLogMaxModelSetSize = 250; //!< default value for switching matching log on
            static constexpr int MaxModelAddedFailoverTrials = 3; //!< if model cannot be added, try again max <n> times

            //  ------------ slots connected with network or other contexts ---------

            //! Remote aircraft added and ready for model matching
            //! \ingroup crosscontextfunction
            void xCtxAddedRemoteAircraftReadyForModelMatching(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

            //! Remove remote aircraft
            //! \ingroup crosscontextfunction
            void xCtxRemovedRemoteAircraft(const swift::misc::aviation::CCallsign &callsign);

            //! Changed remote aircraft model
            //! \ingroup crosscontextfunction
            void xCtxChangedRemoteAircraftModel(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator);

            //! Changed own aircraft model
            //! \ingroup crosscontextfunction
            void xCtxChangedOwnAircraftModel(const swift::misc::simulation::CAircraftModel &aircraftModel, const swift::misc::CIdentifier &originator);

            //! Enable / disable aircraft
            //! \ingroup crosscontextfunction
            void xCtxChangedRemoteAircraftEnabled(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Network connection status
            //! \ingroup crosscontextfunction
            void xCtxNetworkConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

            //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \ingroup crosscontextfunction
            void xCtxUpdateSimulatorCockpitFromContext(const swift::misc::simulation::CSimulatedAircraft &ownAircraft, const swift::misc::CIdentifier &originator);

            //! Update simulator SELCAL from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \ingroup crosscontextfunction
            void xCtxUpdateSimulatorSelcalFromContext(const swift::misc::aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator);

            //! Raw data when a new aircraft was requested, used for statistics
            //! \ingroup crosscontextfunction
            void xCtxNetworkRequestedNewAircraft(const swift::misc::aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

            //! Text message received
            //! \ingroup crosscontextfunction
            void xCtxTextMessagesReceived(const swift::misc::network::CTextMessageList &textMessages);
            //  ------------ slots connected with network or other contexts ---------

            //! Handle new connection status of simulator
            void onSimulatorStatusChanged(ISimulator::SimulatorStatus status);

            //! Listener reports the simulator has started
            void onSimulatorStarted(const swift::misc::simulation::CSimulatorPluginInfo &info);

            //! Simulator model has been changed
            void onOwnSimulatorModelChanged(const swift::misc::simulation::CAircraftModel &model);

            //! Failed adding remote aircraft
            void onAddingRemoteAircraftFailed(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, bool disabled, bool requestFailover, const swift::misc::CStatusMessage &message);

            //! Relay status message to simulator under consideration of settings
            void relayStatusMessageToSimulator(const swift::misc::CStatusMessage &message);

            //! Handle a change in enabled simulators
            void changeEnabledSimulators();

            //! Reads list of enabled simulators, starts listeners
            void restoreSimulatorPlugins();

            //! Load plugin and connect
            bool loadSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorPluginInfo);

            //! Unload plugin, if desired restart listeners
            void unloadSimulatorPlugin();

            //! Listen for single simulator
            bool listenForSimulator(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo);

            //! Listen for all simulators
            void listenForAllSimulators();

            //! Call stop() on all loaded listeners
            void stopSimulatorListeners();

            //! Add to message list for matching
            void addMatchingMessages(const swift::misc::aviation::CCallsign &callsign, const swift::misc::CStatusMessageList &messages);

            //! Clear matching messages
            void clearMatchingMessages(const swift::misc::aviation::CCallsign &callsign);

            //! Reverse lookup of a model
            swift::misc::simulation::CAircraftModel reverseLookupModel(const swift::misc::simulation::CAircraftModel &model);

            //! Load the last know model set
            void initByLastUsedModelSet();

            //! Init and set validator
            void setValidator(const swift::misc::simulation::CSimulatorInfo &simulator);

            QPair<swift::misc::simulation::CSimulatorPluginInfo, QPointer<ISimulator>> m_simulatorPlugin; //!< Currently loaded simulator plugin
            QMap<swift::misc::aviation::CCallsign, swift::misc::CStatusMessageList> m_matchingMessages; //!< all matching log messages per callsign
            QMap<swift::misc::aviation::CCallsign, int> m_failoverAddingCounts;
            CPluginManagerSimulator *m_plugins = nullptr; //!< plugin manager
            swift::misc::CRegularThread m_listenersThread; //!< waiting for plugin
            CAircraftMatcher m_aircraftMatcher { this }; //!< model matcher

            bool m_wasSimulating = false;
            bool m_initallyAddAircraft = false;
            swift::misc::simulation::MatchingLog m_logMatchingMessages = swift::misc::simulation::MatchingLogSimplified;

            QString m_networkSessionId; //!< Network session of CServer::getServerSessionId, if not connected empty (for statistics, ..)
            swift::misc::simulation::CBackgroundValidation *m_validator = nullptr;

            // settings
            swift::misc::CSettingReadOnly<Application::TEnabledSimulators> m_enabledSimulators { this, &CContextSimulator::changeEnabledSimulators };
            swift::misc::CSetting<swift::misc::simulation::settings::TModelMatching> m_matchingSettings { this }; //!< matching settings (all simulators)
            swift::misc::CData<swift::misc::simulation::data::TSimulatorLastSelection> m_modelSetSimulator { this }; //!< current simulator used for model set
            swift::misc::simulation::settings::CMultiSimulatorSettings m_multiSimulatorSettings { this }; //!< individual for simulator, for validation of models
            swift::misc::CSetting<swift::misc::simulation::settings::TInterpolationAndRenderingSetupGlobal> m_renderSettings { this }; //!< rendering/interpolation settings (all simulators)
            swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TSimulatorMessages> m_messageSettings { this }; //!< settings for messages (all simulators)
        };
    } // namespace
} // namespace

#endif // guard
