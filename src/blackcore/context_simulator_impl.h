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

#include "blackcoreexport.h"
#include "blackcore/context_simulator.h"
#include "blackcore/simulator.h"
#include "blackmisc/worker.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/pluginstorageprovider.h"
#include "blackmisc/variant.h"
#include <QTimer>
#include <QDir>
#include <QtConcurrent/QtConcurrent>

namespace BlackCore
{
    //! Network simulator concrete implementation
    class BLACKCORE_EXPORT CContextSimulator :
        public IContextSimulator,
        public BlackMisc::IPluginStorageProvider
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
        friend class CRuntime;
        friend class IContextSimulator;

    public:
        //! Destructor
        virtual ~CContextSimulator();

        //! \copydoc IPluginStorageProvider::getPluginData
        virtual BlackMisc::CVariant getPluginData(const QObject *obj, const QString &key) const;

        //! \copydoc IPluginStorageProvider::setPluginData
        virtual void setPluginData(const QObject *obj, const QString &key, const BlackMisc::CVariant &value);

        //! Gracefully shut down, e.g. for plugin unloading
        void gracefulShutdown();

    public slots:
        //! \copydoc IContextSimulator::getSimulatorPluginInfo()
        virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

        //! \copydoc IContextSimulator::getAvailableSimulatorPlugins()
        virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

        //! \copydoc IContextSimulator::startSimulatorPlugin()
        virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

        //! \copydoc IContextSimulator::stopSimulatorPlugin()
        virtual void stopSimulatorPlugin() override;

        //! \copydoc IContextSimulator::getSimulatorStatus()
        virtual int getSimulatorStatus() const override;

        //! \copydoc IContextSimulator::getSimulatorSetup()
        virtual BlackMisc::Simulation::CSimulatorSetup getSimulatorSetup() const override;

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
        virtual BlackMisc::Aviation::CAircraftIcaoData getIcaoForModelString(const QString &modelString) const override;

        //! \copydoc IContextSimulator::setTimeSynchronization
        virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;

        //! \copydoc IContextSimulator::isTimeSynchronized
        virtual bool isTimeSynchronized() const override;

        //! \copydoc IContextSimulator::getMaxRenderedAircraft
        virtual int getMaxRenderedAircraft() const override;

        //! \copydoc IContextSimulator::setMaxRenderedAircraft
        virtual void setMaxRenderedAircraft(int number) override;

        //! \copydoc IContextSimulator::getMaxRenderedDistance
        virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const override;

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void setMaxRenderedDistance(const BlackMisc::PhysicalQuantities::CLength &distance) override;

        //! \copydoc IContextSimulator::getRenderRestrictionText
        virtual QString getRenderRestrictionText() const override;

        //! \copydoc IContextSimulator::getRenderedDistanceBoundary
        virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const override;

        //! \copydoc IContextSimulator::setMaxRenderedDistance
        virtual void deleteAllRenderingRestrictions() override;

        //! \copydoc IContextSimulator::isRenderingRestricted
        virtual bool isRenderingRestricted() const override;

        //! \copydoc IContextSimulator::isRenderingEnabled
        virtual bool isRenderingEnabled() const override;

        //! \copydoc IContextSimulator::getTimeSynchronizationOffset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

        //! \copydoc IContextSimulator::settingsChanged
        virtual void settingsChanged(uint type) override;

        //! \copydoc IContextSimulator::iconForModel
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        //! \copydoc ISimulator::enableDebuggingMessages
        virtual void enableDebugMessages(bool driver, bool interpolator) override;

        //! \copydoc IContextSimulator::highlightAircraft
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

    protected:
        //! Constructor
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
        void ps_simulatorStarted(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        //! Simulator has changed cockpit
        void ps_cockpitChangedFromSimulator(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

        //! Changed remote aircraft model
        void ps_changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::COriginator &originator);

        //! Enable / disable aircraft
        void ps_changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::COriginator &originator);

        //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
        //! \remarks set by runtime, only to be used locally (not via DBus)
        void ps_updateSimulatorCockpitFromContext(const BlackMisc::Aviation::CAircraft &ownAircraft, const BlackMisc::COriginator &originator);

    private:
        //! A simple struct containing all info about the plugin.
        //! \todo Would we want to use m_member style here?
        struct PluginData
        {
            PluginData(const BlackMisc::Simulation::CSimulatorPluginInfo &info, ISimulatorFactory *factory, ISimulatorListener *listener, ISimulator *simulator, const QString &fileName) :
                info(info), factory(factory), listener(listener), simulator(simulator), fileName(fileName) {}

            BlackMisc::Simulation::CSimulatorPluginInfo info;
            ISimulatorFactory *factory = nullptr;   //!< Lazy-loaded, nullptr by default
            ISimulatorListener *listener = nullptr; //!< Listener instance, nullptr by default
            ISimulator *simulator = nullptr;        //!< The simulator itself (always nullptr unless it is the currently working one)
            QString fileName;                       //!< Plugin file name (relative to plugins/simulator)
            QHash<QString, BlackMisc::CVariant> m_storage; //!< Permanent plugin storage - data stored here will be kept even when plugin is unloaded
        };

        //! Lazy-loads the driver, instantiates the factory and returns it.
        //! \return nullptr if no corresponding driver was found or an error occured during loading it.
        ISimulatorFactory *getSimulatorFactory(const BlackMisc::Simulation::CSimulatorPluginInfo &simulator);

        //! Load plugin, if required start listeners before
        bool loadSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo, bool withListeners);

        //! Unload plugin, if desired restart listeners
        void unloadSimulatorPlugin();

        //! Find and catalog all simulator plugins
        void findSimulatorPlugins();

        //! Load plugin from settings
        bool loadSimulatorPluginFromSettings();

        //! Listen for single simulator
        void listenForSimulator(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo);

        //! Listen for all simulators
        void listenForAllSimulators();

        //! Call stop() on all loaded listeners
        void stopSimulatorListeners();

        //! Locate PluginData (linear search)
        PluginData *findPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        QDir m_pluginsDir;
        QList<PluginData> m_simulatorPlugins;
        PluginData *m_simulatorPlugin = nullptr; //!< Currently loaded simulator plugin
        BlackMisc::CRegularThread m_listenersThread;
    };

} // namespace

#endif // guard
