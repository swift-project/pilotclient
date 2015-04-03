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
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
            IContextSimulator(mode, runtime),
            m_mapper(new QSignalMapper(this))
    {
        findSimulatorPlugins();
        // Maps listener instance
        connect(m_mapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped), this, &CContextSimulator::ps_simulatorStarted);
    }

    CContextSimulator::~CContextSimulator()
    {
        disconnectFromSimulator();
        unloadSimulatorPlugin();
    }
    
    ISimulatorFactory *CContextSimulator::getSimulatorFactory(const CSimulatorPluginInfo &simulator)
    {
        PluginData *plugin = findPlugin(simulator);
        if (!plugin)
            return nullptr;
        
        if (!plugin->factory)
        {
            QPluginLoader loader(plugin->fileName);
            QObject *instance = loader.instance();
            if (instance)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory *>(instance);
                if (factory)
                {
                    plugin->factory = factory;
                    CLogMessage(this).debug() << "Loaded plugin: " << plugin->info.toQString();
                }
            } else {
                QString errorMsg = loader.errorString().append(" ").append("Also check if required dll/libs of plugin exists");
                CLogMessage(this).error(errorMsg);
            }
        }
        
        return plugin->factory;
    }

    CSimulatorPluginInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorPluginInfoList list;

        std::for_each(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [&list](const PluginData &driver)
        {
            list.push_back(driver.info);
        });
        
        return list;
    }

    bool CContextSimulator::isConnected() const
    {

        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->isConnected();
    }

    bool CContextSimulator::canConnect() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->canConnect();
    }

    bool CContextSimulator::disconnectFromSimulator()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->disconnectFrom();
    }

    BlackMisc::Simulation::CSimulatorPluginInfo CContextSimulator::getSimulatorInfo() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return BlackMisc::Simulation::CSimulatorPluginInfo();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->info;
    }

    CAirportList CContextSimulator::getAirportsInRange() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator)
        {
            return CAirportList();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->getAirportsInRange();
    }

    CAircraftModelList CContextSimulator::getInstalledModels() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator)
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->getInstalledModels();
    }

    int CContextSimulator::getInstalledModelsCount() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return 0; }

        Q_ASSERT(m_simulator->simulator);
        return this->m_simulator->simulator->getInstalledModels().size();
    }

    CAircraftModelList CContextSimulator::getInstalledModelsStartingWith(const QString modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulator)
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->getInstalledModels().findModelsStartingWith(modelString);
    }

    void CContextSimulator::reloadInstalledModels()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return;
        }
        Q_ASSERT(m_simulator->simulator);
        m_simulator->simulator->reloadInstalledModels();
    }

    CAircraftIcao CContextSimulator::getIcaoForModelString(const QString &modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulator)
        {
            return CAircraftIcao();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->getIcaoForModelString(modelString);
    }

    bool CContextSimulator::setTimeSynchronization(bool enable, CTime offset)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        bool c = m_simulator->simulator->setTimeSynchronization(enable, offset);
        if (!c)
        {
            return false;
        }

        CLogMessage(this).info(enable ? QStringLiteral("Set time syncronization to %1").arg(offset.toQString()) : QStringLiteral("Disabled time syncrhonization"));
        return true;
    }

    bool CContextSimulator::isTimeSynchronized() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->isTimeSynchronized();
    }

    int CContextSimulator::getMaxRenderedAircraft() const
    {
        if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return 0;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->getMaxRenderedAircraft();
    }

    void CContextSimulator::setMaxRenderedAircraft(int number)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }

        if (m_simulator)
        {
            Q_ASSERT(m_simulator->simulator);
            m_simulator->simulator->setMaxRenderedAircraft(number);
        }
    }

    void CContextSimulator::setMaxRenderedDistance(CLength &distance)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << distance; }
        if (m_simulator)
        {
            Q_ASSERT(m_simulator->simulator);
            this->m_simulator->simulator->setMaxRenderedDistance(distance);
        }
    }

    QString CContextSimulator::getRenderRestrictionText() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator) { return ""; }

        Q_ASSERT(m_simulator->simulator);
        if (!m_simulator->simulator->isRenderingRestricted()) { return "none"; }
        QString rt;
        if (m_simulator->simulator->isMaxAircraftRestricted())
        {
            rt.append(QString::number(m_simulator->simulator->getMaxRenderedAircraft())).append(" A/C");
        }
        if (m_simulator->simulator->isMaxDistanceRestricted())
        {
            if (!rt.isEmpty()) { rt.append(" ");}
            rt.append(m_simulator->simulator->getMaxRenderedDistance().valueRoundedWithUnit(CLengthUnit::NM(), 0));
        }
        return rt;
    }

    CLength CContextSimulator::getMaxRenderedDistance() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return CLength(0, CLengthUnit::nullUnit());
        }
        Q_ASSERT(m_simulator->simulator);
        return this->m_simulator->simulator->getMaxRenderedDistance();
    }

    CLength CContextSimulator::getRenderedDistanceBoundary() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return CLength(20.0, CLengthUnit::NM());
        }
        Q_ASSERT(m_simulator->simulator);
        return this->m_simulator->simulator->getRenderedDistanceBoundary();
    }

    void CContextSimulator::deleteAllRenderingRestrictions()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulator)
        {
            Q_ASSERT(m_simulator->simulator);
            this->m_simulator->simulator->deleteAllRenderingRestrictions();
        }
    }

    bool CContextSimulator::isRenderingRestricted() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return false;
        }
        Q_ASSERT(m_simulator->simulator);
        return this->m_simulator->simulator->isRenderingRestricted();
    }

    CTime CContextSimulator::getTimeSynchronizationOffset() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulator)
        {
            return CTime(0, CTimeUnit::hrmin());
        }
        Q_ASSERT(m_simulator->simulator);
        return this->m_simulator->simulator->getTimeSynchronizationOffset();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        Q_ASSERT(getIContextApplication());
        Q_ASSERT(getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        // warning if we do not have any plugins
        if (m_simulatorPlugins.isEmpty())
        {
            CLogMessage(this).error("No simulator plugins");
            return false;
        }

        ISimulatorFactory *factory = getSimulatorFactory(simulatorInfo);
        Q_ASSERT(factory);

        // We assume we run in the same process as the own aircraft context
        // Hence we pass in memory reference to own aircraft object
        Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());
        Q_ASSERT(this->getIContextNetwork()->isUsingImplementingObject());
        IOwnAircraftProvider *ownAircraftProvider = this->getRuntime()->getCContextOwnAircraft();
        IRemoteAircraftProvider *renderedAircraftProvider = this->getRuntime()->getCContextNetwork();
        ISimulator *newSimulator = factory->create(ownAircraftProvider, renderedAircraftProvider, this);
        Q_ASSERT(newSimulator);

        unloadSimulatorPlugin(); // old plugin unloaded

        PluginData* plugin = findPlugin(simulatorInfo);
        plugin->simulator = newSimulator;
        m_simulator = plugin;

        connect(m_simulator->simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::ps_onSimulatorStatusChanged);
        connect(m_simulator->simulator, &ISimulator::ownAircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        connect(m_simulator->simulator, &ISimulator::modelMatchingCompleted, this, &IContextSimulator::modelMatchingCompleted);
        connect(m_simulator->simulator, &ISimulator::installedAircraftModelsChanged, this, &IContextSimulator::installedAircraftModelsChanged);
        connect(m_simulator->simulator, &ISimulator::restrictedRenderingChanged, this, &IContextSimulator::restrictedRenderingChanged);

        // log from context to simulator
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, m_simulator->simulator, &ISimulator::displayStatusMessage);
        connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged,m_simulator->simulator, &ISimulator::displayStatusMessage);

        // connect with network
        IContextNetwork *networkContext = this->getIContextNetwork();
        Q_ASSERT(networkContext);
        Q_ASSERT(networkContext->isLocalObject());

        for (const CSimulatedAircraft &simAircraft : networkContext->getAircraftInRange())
        {
            Q_ASSERT(!simAircraft.getCallsign().isEmpty());
            m_simulator->simulator->addRemoteAircraft(simAircraft);
        }

        // apply latest settings
        settingsChanged(static_cast<uint>(IContextSettings::SettingsSimulator));

        // try to connect
        m_simulator->simulator->asyncConnectTo();

        // info about what is going on
        CLogMessage(this).info("Simulator plugin loaded: %1") << this->m_simulator->info.toQString(true);
        return true;
    }

    bool CContextSimulator::loadSimulatorPluginFromSettings()
    {
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSettings()) return false; // TODO assert or if?

        // TODO warnings if we didn't load the plugin which the settings asked for

        CSimulatorPluginInfoList plugins = this->getAvailableSimulatorPlugins();
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
    
    void CContextSimulator::listenForSimulator(const CSimulatorPluginInfo &simulatorInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        if (this->m_simulator)
        {  // already loaded
            qWarning("Cannot listen for simulator while the driver is still loaded");
            return;
        }

        // warning if we do not have any plugins
        if (m_simulatorPlugins.isEmpty())
        {
            CLogMessage(this).error("No simulator drivers available");
            return;
        }

        PluginData* plugin = findPlugin(simulatorInfo);
        if (!plugin)
        {
            CLogMessage(this).error("Driver not found for '%1'") << simulatorInfo.toQString();
            return;
        }

        if (!plugin->listener)
        {
            ISimulatorFactory* factory = getSimulatorFactory(simulatorInfo);
            Q_ASSERT(factory);

            plugin->listener = factory->createListener();
            connect(plugin->listener, SIGNAL(simulatorStarted()), m_mapper, SLOT(map()));
            m_mapper->setMapping(plugin->listener, plugin->listener);
            plugin->listener->moveToThread(&m_listenersThread);
        }

        ISimulatorListener *listener = plugin->listener;
        Q_ASSERT(listener);

        if (!m_listenersThread.isRunning())
        {
            m_listenersThread.start(QThread::LowPriority);
        }

        QMetaObject::invokeMethod(listener, "start");
        CLogMessage(this).debug() << "Listening for simulator:" << simulatorInfo.toQString(true);
    }
    
    void CContextSimulator::listenForAllSimulators()
    {
        auto plugins = getAvailableSimulatorPlugins();
        for (const auto &p: plugins)
            listenForSimulator(p);
    }
    
    void CContextSimulator::listenForSimulatorFromSettings()
    {
        Q_ASSERT(this->getIContextSettings());

        auto plugin = getIContextSettings()->getSimulatorSettings().getSelectedPlugin();
        if (plugin.isUnspecified())
        {
            listenForAllSimulators();
        }
        else
        {
            listenForSimulator(plugin);
        }
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (m_simulator)
        {
            // depending on shutdown order, network might already have been deleted
            IContextNetwork *networkContext = this->getIContextNetwork();
            Q_ASSERT(networkContext);
            Q_ASSERT(networkContext->isLocalObject());
            Q_UNUSED(networkContext);
            Q_ASSERT(m_simulator->simulator);
            
            m_simulator->simulator->disconnect();
            CLogHandler::instance()->disconnect(m_simulator->simulator);
            this->disconnect(m_simulator->simulator);

            if (m_simulator->simulator->isConnected())
                m_simulator->simulator->disconnectFrom(); // disconnect from simulator

            m_simulator->simulator->deleteLater();
            m_simulator->simulator = nullptr;
            m_simulator = nullptr;
        }
    }

    void CContextSimulator::ps_addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if(!m_simulator)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulator->simulator);
        Q_ASSERT(!remoteAircraft.getCallsign().isEmpty());

        m_simulator->simulator->addRemoteAircraft(remoteAircraft);
    }

    void CContextSimulator::ps_removedRemoteAircraft(const CCallsign &callsign)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if(!m_simulator)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulator->simulator);

        m_simulator->simulator->removeRemoteAircraft(callsign);
    }

    void CContextSimulator::ps_onSimulatorStatusChanged(int status)
    {
        Q_ASSERT(m_simulator);
        Q_ASSERT(m_simulator->simulator);

        if (!(status & ISimulator::Connected))
        {
            unloadSimulatorPlugin();
            listenForSimulatorFromSettings();
        }
        emit simulatorStatusChanged(status);
    }

    void CContextSimulator::ps_textMessagesReceived(const Network::CTextMessageList &textMessages)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if(!m_simulator)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulator->simulator);
        for (const auto &tm : textMessages)
        {
            m_simulator->simulator->displayTextMessage(tm);
        }
    }

    void CContextSimulator::ps_cockitChangedFromSim(const CSimulatedAircraft &ownAircraft)
    {
        Q_ASSERT(getIContextOwnAircraft());

        getIContextOwnAircraft()->changedAircraftCockpit(ownAircraft, IContextSimulator::InterfaceName());
    }

    void CContextSimulator::ps_changedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        this->m_simulator->simulator->changeRemoteAircraftModel(aircraft, originator);
    }

    void CContextSimulator::ps_changedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);
        this->m_simulator->simulator->changeRemoteAircraftEnabled(aircraft, originator);
    }

    void CContextSimulator::ps_updateSimulatorCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if(!m_simulator)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulator->simulator);

        // avoid loops
        if (originator.isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

        // update
        this->m_simulator->simulator->updateOwnSimulatorCockpit(ownAircraft, originator);
    }

    void CContextSimulator::settingsChanged(uint type)
    {
        auto settingsType = static_cast<IContextSettings::SettingsType>(type);
        if (settingsType != IContextSettings::SettingsSimulator)
            return;

        // plugin
        CSettingsSimulator settingsSim = this->getIContextSettings()->getSimulatorSettings();
        CSimulatorPluginInfo plugin = getIContextSettings()->getSimulatorSettings().getSelectedPlugin();
        
        // no simulator loaded yet, listen
        if (!m_simulator)
        {
            stopSimulatorListeners();
            if (plugin.isUnspecified())
            {
                listenForAllSimulators();
            }
            else
            {
                listenForSimulator(plugin);
            }
        }
        else
        {
            // time sync
            bool timeSync = settingsSim.isTimeSyncEnabled();
            CTime syncOffset = settingsSim.getSyncTimeOffset();
            m_simulator->simulator->setTimeSynchronization(timeSync, syncOffset);
        }
    }

    CPixmap CContextSimulator::iconForModel(const QString &modelString) const
    {
        if (!this->m_simulator)
        {
            return CPixmap();
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->iconForModel(modelString);
    }

    void CContextSimulator::enableDebugMessages(bool driver, bool interpolator)
    {
        if (!this->m_simulator)
        {
            return;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->enableDebugMessages(driver, interpolator);
    }

    void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        Q_ASSERT(m_simulator->simulator);
        this->m_simulator->simulator->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
    }

    bool CContextSimulator::isPaused() const
    {
        if (!this->m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->isPaused();
    }

    bool CContextSimulator::isSimulating() const
    {
        if (!this->m_simulator)
        {
            return false;
        }

        Q_ASSERT(m_simulator->simulator);
        return m_simulator->simulator->isSimulating();
    }
    
    void CContextSimulator::ps_simulatorStarted(QObject *listener)
    {
        Q_ASSERT(listener);
        Q_ASSERT(listener->inherits("BlackCore::ISimulatorListener"));

        /* Find caller */
        PluginData *plugin = [this](QObject *listener)
        {
            auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [listener](const PluginData &plugin)
            {
            return plugin.listener == listener;
            });
            return &(*it);
        }(listener);
        Q_ASSERT(plugin);

        CLogMessage(this).debug() << plugin->info.toQString() << "started";
        stopSimulatorListeners();
        loadSimulatorPlugin(plugin->info);
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
        for (const auto& fileName: fileNames)
        {
            if (!QLibrary::isLibrary(fileName))
            {
                continue;
            }

            CLogMessage(this).debug() << "Try to load plugin: " << fileName;
            QString pluginPath = m_pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginPath);
            QJsonObject json = loader.metaData();
            CSimulatorPluginInfo simulatorPluginInfo;
            simulatorPluginInfo.convertFromJson(json);
            if (simulatorPluginInfo.isValid())
            {
                m_simulatorPlugins << PluginData{ simulatorPluginInfo, nullptr, nullptr, nullptr, pluginPath};
                CLogMessage(this).debug() << "Found simulator driver: " << simulatorPluginInfo.toQString();
            }
            else
            {
                CLogMessage(this).warning("Simulator driver in %1 is invalid") << pluginPath;
            }
        }
    }
    
    void CContextSimulator::stopSimulatorListeners()
    {
        std::for_each(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [](PluginData& plugin)
        {
            if (plugin.listener)
                QMetaObject::invokeMethod(plugin.listener, "stop");
        });
    }

    CContextSimulator::PluginData *CContextSimulator::findPlugin(const CSimulatorPluginInfo &info)
    {
        auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [&info](PluginData& plugin)
        {
            return plugin.info == info;
        });

        return &(*it);
    }

} // namespace
