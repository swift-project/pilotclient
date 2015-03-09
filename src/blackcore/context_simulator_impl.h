/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/context_simulator.h"
#include "blackcore/simulator.h"
#include "blackmisc/worker.h"
#include "blacksim/simulatorinfo.h"
#include "blacksim/simulatorinfolist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include <QTimer>
#include <QDir>
#include <QtConcurrent/QtConcurrent>

namespace BlackCore
{
    /*!
     * Network simulator concrete implementation
     */
    class CContextSimulator : public IContextSimulator
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
        friend class CRuntime;
        friend class IContextSimulator;

    public:
        //! Destructor
        virtual ~CContextSimulator();
        
        //! Lazy-loads the driver, instantiates the factory and returns it.
        //! \return nullptr if no corresponding driver was found or an error occured during loading it.
        //! \todo Consider moving to private scope.
        ISimulatorFactory* getSimulatorFactory(const BlackSim::CSimulatorInfo& simulator);

    public slots:

        //! \copydoc IContextSimulator::getSimulatorPluginList()
        virtual BlackSim::CSimulatorInfoList getAvailableSimulatorPlugins() const override;

        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextSimulator::canConnect
        virtual bool canConnect() const override;

        //! \copydoc IContextSimulator::connectTo
        virtual bool connectToSimulator() override;

        //! \copydoc IContextSimulator::asyncConnectTo
        virtual void asyncConnectToSimulator() override;

        //! \copydoc IContextSimulator::disconnectFrom
        virtual bool disconnectFromSimulator() override;

        //! \copydoc IContextSimulator::isPaused
        virtual bool isPaused() const override;

        //! \copydoc IContextSimulator::isSimulating
        virtual bool isSimulating() const override;

        //! \copydoc IContextSimulator::getSimulatorInfo()
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

        //! \copydoc IContextSimulator::getAirportsInRange
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

        //! \copydoc IContextSimulator::getInstalledModels
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

        //! \copydoc IContextSimulator::getInstalledModelsCount
        virtual int getInstalledModelsCount() const override;

        //! \copydoc IContextSimulator::getInstalledModelsStartingWith
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModelsStartingWith(const QString modelString) const override;

        //! \copydoc IContextSimulator::reloadInstalledModels
        virtual void reloadInstalledModels() override;

        //! \copydoc IContextSimulator::getIcaoForModelString
        virtual BlackMisc::Aviation::CAircraftIcao getIcaoForModelString(const QString &modelString) const override;

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override;

        //! \copydoc IContextSimulator::getMaxRenderedAircraft
        virtual int getMaxRenderedAircraft() const override;

        //! \copydoc IContextSimulator::setMaxRenderedAircraft
        virtual void setMaxRenderedAircraft(int number) override;

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void setMaxRenderedDistance(BlackMisc::PhysicalQuantities::CLength &distance) override;

        //! \copydoc IContextSimulator::getRenderRestrictionText
        virtual QString getRenderRestrictionText() const override;

        //! \copydoc IContextSimulator::getMaxRenderedDistance
        virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const override;

        //! \copydoc IContextSimulator::getRenderedDistanceBoundary
        virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const override;

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void deleteAllRenderingRestrictions() override;

        //! \copydoc IContextSimulator::isRenderingRestricted
        virtual bool isRenderingRestricted() const override;

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

        //! \copydoc IContextSimulator::loadSimulatorPlugin()
        virtual bool loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::loadSimulatorPluginFromSettings()
        virtual bool loadSimulatorPluginFromSettings() override;
        
        //! \copydoc IContextSimulator::listenForSimulator()
        virtual void listenForSimulator(const BlackSim::CSimulatorInfo &simulatorInfo) override;
        
        //! \copydoc IContextSimulator::listenForAllSimulators()
        virtual void listenForAllSimulators() override;
        
        //! \copydoc IContextSimulator::listenForSimulatorFromSettings()
        virtual void listenForSimulatorFromSettings() override;

        //! \copydoc IContextSimulator::unloadSimulatorPlugin()
        virtual void unloadSimulatorPlugin() override;

        //! \copydoc IContextSimulator::settingsChanged
        virtual void settingsChanged(uint type) override;

        //! \copydoc IContextSimulator::iconForModel
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        //! \copydoc ISimulator::enableDebuggingMessages
        virtual void enableDebugMessages(bool driver, bool interpolator) override;

        //! \copydoc IContextSimulator::highlightAircraft
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

    protected:
        //! \brief Constructor
        CContextSimulator(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextSimulator *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(CContextSimulator::ObjectPath(), this);
            return this;
        }

    private slots:
        //! Remote aircraft added
        void ps_addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! Remove remote aircraft
        void ps_removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Handle new connection status of simulator
        void ps_onSimulatorStatusChanged(int status);

        //! Text message received
        void ps_textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);
        
        //! Listener reports the simulator has started
        void ps_simulatorStarted(BlackSim::CSimulatorInfo simulatorInfo);

        //! Simulator has changed cockpit
        void ps_cockitChangedFromSim(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

        //! Changed remote aircraft model
        void ps_changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator);

        //! Enable / disable aircraft
        void ps_changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator);

        //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
        //! \remarks set by runtime, only to be used locally (not via DBus)
        void ps_updateSimulatorCockpitFromContext(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator);

    private:
        //! \brief find and catalog all simulator plugins
        void findSimulatorPlugins();

        //! \brief call stop() on all loaded listeners
        void stopSimulatorListeners();
        
        /*!
         * A simple struct containing all info about the driver we need.
         */
        struct DriverInfo {
            ISimulatorFactory* factory; //!< Lazy-loaded, nullptr by default
            ISimulatorListener* listener; //!< Listener instance, nullptr by default
            QString fileName; //!< Plugin file name (relative to plugins/simulator)
        };

        BlackCore::ISimulator *m_simulator = nullptr; //!< Actually loaded simulator driver
        QTimer *m_updateTimer = nullptr;
        QDir m_pluginsDir;
        QMap<BlackSim::CSimulatorInfo, DriverInfo> m_simulatorDrivers;
        QFuture<bool> m_canConnectResult;
        BlackMisc::CRegularThread m_listenersThread;
    };

} // namespace

#endif // guard
