/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context_simulator_impl.h"
#include "context_ownaircraft_impl.h"
#include "context_ownaircraft.h"
#include "context_settings.h"
#include "context_application.h"
#include "context_network_impl.h"
#include "context_runtime.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include <QPluginLoader>
#include <QLibrary>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackSim;
using namespace BlackSim::Settings;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime)
    {
        findSimulatorPlugins();

        // do not load plugin here, as it depends on settings
        // it has to be guaranteed the settings are alredy loaded
    }

    CContextSimulator::~CContextSimulator()
    {
        disconnectFromSimulator();
        unloadSimulatorPlugin();
    }

    CSimulatorInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorInfoList simulatorPlugins;
        foreach(ISimulatorFactory * factory, m_simulatorFactories)
        {
            simulatorPlugins.push_back(factory->getSimulatorInfo());
        }
        simulatorPlugins.sortBy(&CSimulatorInfo::getShortName);
        return simulatorPlugins;
    }

    bool CContextSimulator::isConnected() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return false;
        return m_simulator->isConnected();
    }

    bool CContextSimulator::canConnect() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return false;
        return m_simulator->canConnect();
    }

    bool CContextSimulator::connectToSimulator()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return false;
        return m_simulator->connectTo();
    }

    void CContextSimulator::asyncConnectToSimulator()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator || m_canConnectResult.isRunning()) return; // already checking
        m_simulator->asyncConnectTo();
    }

    bool CContextSimulator::disconnectFromSimulator()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return false;
        return m_simulator->disconnectFrom();
    }

    BlackSim::CSimulatorInfo CContextSimulator::getSimulatorInfo() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return BlackSim::CSimulatorInfo::UnspecifiedSim();
        return m_simulator->getSimulatorInfo();
    }

    CAirportList CContextSimulator::getAirportsInRange() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator) { return CAirportList(); }

        return this->m_simulator->getAirportsInRange();
    }

    CAircraftModelList CContextSimulator::getInstalledModels() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator) { return CAircraftModelList(); }

        return this->m_simulator->getInstalledModels();
    }

    int CContextSimulator::getInstalledModelsCount() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return 0; }

        return this->m_simulator->getInstalledModels().size();
    }

    CAircraftModelList CContextSimulator::getInstalledModelsStartingWith(const QString modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulator) { return CAircraftModelList(); }
        return this->m_simulator->getInstalledModels().findModelsStartingWith(modelString);
    }

    void CContextSimulator::reloadInstalledModels()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return; }
        m_simulator->reloadInstalledModels();
    }

    CAircraftIcao CContextSimulator::getIcaoForModelString(const QString &modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulator) { return CAircraftIcao(); }
        return this->m_simulator->getIcaoForModelString(modelString);
    }

    bool CContextSimulator::setTimeSynchronization(bool enable, CTime offset)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return false; }
        bool c = this->m_simulator->setTimeSynchronization(enable, offset);
        if (!c) { return false; }
        if (enable)
        {
            CLogMessage(this).info("Set time syncronization to %1") << offset.toQString();
        }
        else
        {
            CLogMessage(this).info("Disabled time syncronization %1");
        }
        return true;
    }

    bool CContextSimulator::isTimeSynchronized() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return false;
        return this->m_simulator->isTimeSynchronized();
    }

    int CContextSimulator::getMaxRenderedAircraft() const
    {
        if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return 0;
        return m_simulator->getMaxRenderedAircraft();
    }

    void CContextSimulator::setMaxRenderedAircraft(int number)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }
        if (m_simulator) { this->m_simulator->setMaxRenderedAircraft(number); }
    }

    void CContextSimulator::setMaxRenderedDistance(CLength &distance)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << distance; }
        if (m_simulator) { this->m_simulator->setMaxRenderedDistance(distance); }
    }

    QString CContextSimulator::getRenderRestrictionText() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return ""; }
        if (!m_simulator->isRenderingRestricted()) { return "none"; }
        QString rt;
        if (m_simulator->isMaxAircraftRestricted())
        {
            rt.append(QString::number(m_simulator->getMaxRenderedAircraft())).append(" A/C");
        }
        if (m_simulator->isMaxDistanceRestricted())
        {
            if (!rt.isEmpty()) { rt.append(" ");}
            rt.append(m_simulator->getMaxRenderedDistance().valueRoundedWithUnit(CLengthUnit::NM(), 0));
        }
        return rt;
    }

    CLength CContextSimulator::getMaxRenderedDistance() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulator) { return this->m_simulator->getMaxRenderedDistance(); }
        return CLength(0, CLengthUnit::nullUnit());
    }

    CLength CContextSimulator::getRenderedDistanceBoundary() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulator) { return this->m_simulator->getRenderedDistanceBoundary(); }
        return CLength(20.0, CLengthUnit::NM());
    }

    void CContextSimulator::deleteAllRenderingRestrictions()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulator) { this->m_simulator->deleteAllRenderingRestrictions(); }
    }

    bool CContextSimulator::isRenderingRestricted() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulator) { return this->m_simulator->isRenderingRestricted(); }
        return false;
    }

    CTime CContextSimulator::getTimeSynchronizationOffset() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) return CTime(0, CTimeUnit::hrmin());
        return this->m_simulator->getTimeSynchronizationOffset();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorInfo &simulatorInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());

        if (this->m_simulator && this->m_simulator->getSimulatorInfo() == simulatorInfo) { return true; } // already loaded
        if (simulatorInfo.isUnspecified()) { return false; }

        // warning if we do not have any plugins
        if (m_simulatorFactories.isEmpty())
        {
            CLogMessage(this).error("No simulator plugins");
            return false;
        }

        auto factoryIterator = std::find_if(m_simulatorFactories.begin(), m_simulatorFactories.end(), [ = ](const ISimulatorFactory * factory)
        {
            return factory->getSimulatorInfo() == simulatorInfo;
        });

        // no plugin found
        if (factoryIterator == m_simulatorFactories.end())
        {
            CLogMessage(this).error("Plugin not found: '%1'") << simulatorInfo.toQString(true);
            return false;
        }

        // Factory for driver
        ISimulatorFactory *factory = *factoryIterator;
        Q_ASSERT(factory);

        // We assume we run in the same process as the own aircraft context
        // Hence we pass in memory reference to own aircraft object
        Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());
        Q_ASSERT(this->getIContextNetwork()->isUsingImplementingObject());
        IOwnAircraftProvider *ownAircraftProvider = this->getRuntime()->getCContextOwnAircraft();
        IRemoteAircraftProvider *renderedAircraftProvider = this->getRuntime()->getCContextNetwork();
        ISimulator *newSimulator = factory->create(ownAircraftProvider, renderedAircraftProvider, this);
        Q_ASSERT(newSimulator);

        this->unloadSimulatorPlugin(); // old plugin unloaded
        m_simulator = newSimulator;

        connect(m_simulator, &ISimulator::connectionStatusChanged, this, &CContextSimulator::ps_onConnectionStatusChanged);
        connect(m_simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::simulatorStatusChanged);
        connect(m_simulator, &ISimulator::ownAircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        connect(m_simulator, &ISimulator::modelMatchingCompleted, this, &IContextSimulator::modelMatchingCompleted);
        connect(m_simulator, &ISimulator::installedAircraftModelsChanged, this, &IContextSimulator::installedAircraftModelsChanged);
        connect(m_simulator, &ISimulator::restrictedRenderingChanged, this, &IContextSimulator::restrictedRenderingChanged);

        // log from context to simulator
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, m_simulator, &ISimulator::displayStatusMessage);
        connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, m_simulator, &ISimulator::displayStatusMessage);

        // connect with network
        IContextNetwork *networkContext = this->getIContextNetwork();
        Q_ASSERT(networkContext);
        Q_ASSERT(networkContext->isLocalObject());

        // use readyForModelMatching instead of CAirspaceMonitor::addedAircraft, as it contains client information
        // ready for model matching is sent delayed when all information are available
        bool c = connect(networkContext, &IContextNetwork::readyForModelMatching, this, &CContextSimulator::ps_addRemoteAircraft);
        Q_ASSERT(c);
        c = connect(networkContext, &IContextNetwork::removedAircraft, this, &CContextSimulator::ps_removedRemoteAircraft);
        Q_ASSERT(c);
        c = connect(networkContext, &IContextNetwork::changedRemoteAircraftModel, this, &CContextSimulator::ps_changedRemoteAircraftModel);
        Q_ASSERT(c);
        c = connect(networkContext, &IContextNetwork::changedRemoteAircraftEnabled, this, &CContextSimulator::ps_changedRemoteAircraftEnabled);
        Q_ASSERT(c);
        Q_UNUSED(c);

        for (const CSimulatedAircraft &simAircraft : networkContext->getAircraftInRange())
        {
            Q_ASSERT(!simAircraft.getCallsign().isEmpty());
            m_simulator->addRemoteAircraft(simAircraft);
        }

        // apply latest settings
        this->settingsChanged(static_cast<uint>(IContextSettings::SettingsSimulator));

        // try to connect
        asyncConnectToSimulator();

        // info about what is going on
        CLogMessage(this).info("Simulator plugin loaded: '%1'") << this->m_simulator->getSimulatorInfo().toQString(true);
        return true;
    }

    bool CContextSimulator::loadSimulatorPluginFromSettings()
    {
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSettings()) { return false; }

        // TODO warnings if we didn't load the plugin which the settings asked for

        CSimulatorInfoList plugins = this->getAvailableSimulatorPlugins();
        if (plugins.size() == 1)
        {
            // load, independent from settings, we have only driver
            return this->loadSimulatorPlugin(plugins.front());
        }
        else if (plugins.size() > 1)
        {
            if (this->loadSimulatorPlugin(
                        this->getIContextSettings()->getSimulatorSettings().getSelectedPlugin()
                    )) return true;

            // we have plugins, but none got loaded
            // just load first one
            return this->loadSimulatorPlugin(plugins.front());
        }
        else
        {
            return false;
        }
    }
    
    void CContextSimulator::listenForSimulator(const CSimulatorInfo &simulatorInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());

        if (this->m_simulator && this->m_simulator->getSimulatorInfo() == simulatorInfo) {  // already loaded
            qWarning("Cannot listen for simulator while still plugin is loaded");
            return;
        }
        
        if (simulatorInfo.isUnspecified()) {
            return;
        }

        // warning if we do not have any plugins
        if (m_simulatorListeners.isEmpty()) {
            CLogMessage(this).error("No simulator listeners");
            return;
        }

        if (!m_simulatorListeners.contains(simulatorInfo)) {
            CLogMessage(this).error("Listener not found for '%1'") << simulatorInfo.toQString(true);
            return;
        } else {
            ISimulatorListener *listener = m_simulatorListeners[simulatorInfo];
            Q_ASSERT(listener);
            listener->start();
            CLogMessage(this).info("Listening for simulator: '%1'") << simulatorInfo.toQString(true);
        }
        
    }
    
    void CContextSimulator::listenForSimulatorFromSettings()
    {
        Q_ASSERT(this->getIContextSettings());
        listenForSimulator(getIContextSettings()->getSimulatorSettings().getSelectedPlugin());
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (this->m_simulator)
        {
            // depending on shutdown order, network might already have been deleted
            IContextNetwork *networkContext = this->getIContextNetwork();
            Q_ASSERT(networkContext);
            Q_ASSERT(networkContext->isLocalObject());
            Q_UNUSED(networkContext);
            this->m_simulator->disconnect(); // disconnect all simulator signals
            QObject::disconnect(this, nullptr, this->m_simulator, nullptr); // disconnect receiver simulator
            this->m_simulator->disconnectFrom(); // disconnect from simulator
            this->m_simulator->deleteLater();
        }
        this->m_simulator = nullptr;
    }

    void CContextSimulator::ps_addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        Q_ASSERT(this->m_simulator);
        Q_ASSERT(!remoteAircraft.getCallsign().isEmpty());
        if (!this->m_simulator) { return; }
        this->m_simulator->addRemoteAircraft(remoteAircraft);
    }

    void CContextSimulator::ps_removedRemoteAircraft(const CCallsign &callsign)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) { return; }
        this->m_simulator->removeRemoteAircraft(callsign);
    }

    void CContextSimulator::ps_onConnectionStatusChanged(ISimulator::ConnectionStatus status)
    {
        bool connected;
        if (status == ISimulator::Connected)
        {
            connected = true;
        }
        else
        {
            connected = false;
        }
        emit connectionChanged(connected);
    }

    void CContextSimulator::ps_textMessagesReceived(const Network::CTextMessageList &textMessages)
    {
        if (!this->m_simulator) { return; }
        foreach(CTextMessage tm, textMessages)
        {
            this->m_simulator->displayTextMessage(tm);
        }
    }

    void CContextSimulator::ps_cockitChangedFromSim(const CSimulatedAircraft &ownAircraft)
    {
        Q_ASSERT(this->getIContextOwnAircraft());
        if (!this->getIContextOwnAircraft()) { return; }
        this->getIContextOwnAircraft()->changedAircraftCockpit(ownAircraft, IContextSimulator::InterfaceName());
    }

    void CContextSimulator::ps_changedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) { return; }

        this->m_simulator->changeRemoteAircraftModel(aircraft, originator);
    }

    void CContextSimulator::ps_changedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) { return; }

        this->m_simulator->changeRemoteAircraftEnabled(aircraft, originator);
    }

    void CContextSimulator::ps_updateSimulatorCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        if (!this->m_simulator) { return; }

        // avoid loops
        if (originator.isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

        // update
        this->m_simulator->updateOwnSimulatorCockpit(ownAircraft, originator);
    }

    void CContextSimulator::settingsChanged(uint type)
    {
        Q_ASSERT(this->getIContextSettings());
        Q_ASSERT(this->m_simulator);
        if (!this->getIContextSettings()) return;
        auto settingsType = static_cast<IContextSettings::SettingsType>(type);
        if (settingsType != IContextSettings::SettingsSimulator) return;

        // plugin
        CSettingsSimulator settingsSim = this->getIContextSettings()->getSimulatorSettings();
        CSimulatorInfo plugin = settingsSim.getSelectedPlugin();
        if (!this->getSimulatorInfo().isSameSimulator(plugin))
        {
            if (this->loadSimulatorPlugin(plugin))
            {
                CLogMessage(this).info("Plugin loaded: '%1'") << plugin.toQString(true);
            }
            else
            {
                CLogMessage(this).error("Cannot load driver: '%1'") << plugin.toQString(true);
            }
        }

        // time sync
        bool timeSync = settingsSim.isTimeSyncEnabled();
        CTime syncOffset = settingsSim.getSyncTimeOffset();
        this->m_simulator->setTimeSynchronization(timeSync, syncOffset);
    }

    CPixmap CContextSimulator::iconForModel(const QString &modelString) const
    {
        if (!this->m_simulator) { return CPixmap(); }
        return this->m_simulator->iconForModel(modelString);
    }

    void CContextSimulator::enableDebugMessages(bool driver, bool interpolator)
    {
        if (!this->m_simulator) { return; }
        return this->m_simulator->enableDebugMessages(driver, interpolator);
    }

    void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        if (!this->m_simulator) { return; }
        this->m_simulator->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
    }

    bool CContextSimulator::isPaused() const
    {
        if (!this->m_simulator) return false;
        return this->m_simulator->isPaused();
    }

    bool CContextSimulator::isSimulating() const
    {
        if (!this->m_simulator) return false;
        return this->m_simulator->isSimulating();
    }
    
    void CContextSimulator::ps_simulatorStarted(CSimulatorInfo simulatorInfo)
    {
        CLogMessage(this).info("Simulator %1 started.") << simulatorInfo.toQString();
        loadSimulatorPlugin(simulatorInfo);
    }

    void CContextSimulator::findSimulatorPlugins()
    {
        const QString path = qApp->applicationDirPath().append("/plugins/simulator");
        m_pluginsDir = QDir(path);
        if (!m_pluginsDir.exists())
        {
            CLogMessage(this).error("No plugin directory: %1") << m_pluginsDir.currentPath();
            return;
        }

        QStringList fileNames = m_pluginsDir.entryList(QDir::Files);
        fileNames.sort(Qt::CaseInsensitive); // give a certain order, rather than random file order
        foreach(QString fileName, fileNames)
        {
            if (!QLibrary::isLibrary(fileName)) { continue; }
            CLogMessage(this).info("Try to load plugin: %1") << fileName;
            QString pluginPath = m_pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginPath);
            QObject *plugin = loader.instance();
            if (plugin)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory *>(plugin);
                if (factory)
                {
                    CSimulatorInfo simulatorInfo = factory->getSimulatorInfo();
                    m_simulatorFactories.insert(factory);
                    
                    ISimulatorListener *listener = factory->createListener(this);
                    Q_ASSERT(listener);
                    Q_ASSERT(listener->parent() == this); // requirement
                    m_simulatorListeners.insert(simulatorInfo, listener);
                    
                    /* Will not happen unless start() is called */
                    connect(listener, &ISimulatorListener::simulatorStarted, this, &CContextSimulator::ps_simulatorStarted);
        
                    CLogMessage(this).info("Loaded plugin: %1") << simulatorInfo.toQString();
                }
            }
            else
            {
                QString errorMsg = loader.errorString().append(" ").append("Also check if required dll/libs of plugin exists");
                CLogMessage(this).error(errorMsg);
            }
        }
    }

} // namespace
