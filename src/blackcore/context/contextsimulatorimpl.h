/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H


#include "blackcore/aircraftmatcher.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/weathermanager.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorsetup.h"
#include "blackmisc/simulation/simulationsettings.h"
#include "blackmisc/worker.h"

#include <QObject>
#include <QPair>
#include <QString>

namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    class CCoreFacade;
    class CPluginManagerSimulator;
    class ISimulator;

    namespace Context
    {
        //! Network simulator concrete implementation
        class BLACKCORE_EXPORT CContextSimulator :
            public IContextSimulator,
            public BlackMisc::Simulation::CRemoteAircraftAware, // gain access to in memory remote aircraft data
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
            friend class BlackCore::CCoreFacade;
            friend class IContextSimulator;

        public:
            //! Destructor
            virtual ~CContextSimulator();

            //! Gracefully shut down, e.g. for plugin unloading
            void gracefulShutdown();

        public slots:
            //! \name Interface implementations
            //! @{
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;
            virtual int getSimulatorStatus() const override;
            virtual BlackMisc::Simulation::CSimulatorSetup getSimulatorSetup() const override;
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;
            virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const override;
            virtual QStringList getModelSetStrings() const override;
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;
            virtual int getModelSetCount() const override;
            virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString modelString) const override;
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
            virtual bool isTimeSynchronized() const override;
            virtual BlackMisc::CInterpolationAndRenderingSetup getInterpolationAndRenderingSetup() const override;
            virtual void setInterpolationAndRenderingSetup(const BlackMisc::CInterpolationAndRenderingSetup &setup) override;
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
            virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier) override;
            virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isMatchingMessagesEnabled() const override;
            virtual void enableMatchingMessages(bool enabled) override;
            //! @}

        protected:
            //! Constructor
            CContextSimulator(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextSimulator *registerWithDBus(BlackMisc::CDBusServer *server);

        private slots:

            //
            //  ------------ slots connected with network context and not to be used other by network contect  ---------
            //
            //! \ingroup crosscontextslot
            //! @{

            //! Remote aircraft added
            void ps_addedRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! Remove remote aircraft
            void ps_removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! Changed remote aircraft model
            void ps_changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Enable / disable aircraft
            void ps_changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! @}
            //
            //  ------------ slots connected with network context and not to be used other by network contect  ---------
            //

            //! Handle new connection status of simulator
            void ps_onSimulatorStatusChanged(int status);

            //! Model set from model set loader changed
            void ps_modelSetChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Text message received
            void ps_textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

            //! Listener reports the simulator has started
            void ps_simulatorStarted(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

            //! Simulator has changed cockpit
            void ps_cockpitChangedFromSimulator(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            //! Failed adding remote aircraft
            void ps_addingRemoteAircraftFailed(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::CStatusMessage &message);

            //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \ingroup crosscontextslot
            void ps_updateSimulatorCockpitFromContext(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator);

            //! Relay status message to simulator under consideration of settings
            void ps_relayStatusMessageToSimulator(const BlackMisc::CStatusMessage &message);

        private:
            //! Reads list of enabled simulators, starts listeners
            void restoreSimulatorPlugins();

            //! Load plugin, if required start listeners before
            bool loadSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorPluginInfo, bool withListeners);

            //! Unload plugin, if desired restart listeners
            void unloadSimulatorPlugin();

            //! Listen for single simulator
            void listenForSimulator(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo);

            //! Listen for all simulators
            void listenForAllSimulators();

            //! Call stop() on all loaded listeners
            void stopSimulatorListeners();

            //! Add to message list for matching
            void addMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CStatusMessageList &messages);

            QPair<BlackMisc::Simulation::CSimulatorPluginInfo, ISimulator *> m_simulatorPlugin; //!< Currently loaded simulator plugin
            CPluginManagerSimulator *m_plugins = nullptr;
            BlackMisc::CRegularThread m_listenersThread; //!< waiting for plugin
            BlackCore::CWeatherManager m_weatherManager { this };
            BlackMisc::CSetting<BlackCore::Application::TEnabledSimulators> m_enabledSimulators { this, &CContextSimulator::restoreSimulatorPlugins };
            BlackCore::CAircraftMatcher m_modelMatcher; //!< Model matcher
            BlackMisc::Simulation::CAircraftModelSetLoader m_modelSetLoader { this }; //!< load model set from caches
            QMap<BlackMisc::Aviation::CCallsign, BlackMisc::CStatusMessageList> m_matchingMessages;
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::TSimulatorMessages> m_messageSettings { this }; //!< settings for messages
            bool m_initallyAddAircrafts = false;
            bool m_enableMatchingMessages = true;
        };
    } // namespace
} // namespace

#endif // guard
