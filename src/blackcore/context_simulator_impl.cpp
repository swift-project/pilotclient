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
        if (!plugin) { return nullptr; }

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
                    CLogMessage(this).info("Loaded driver: %1") << plugin->info.toQString();
                }
            }
            else
            {
                QString errorMsg = loader.errorString().append(" ").append("Also check if required dll/libs of plugin exists");
                CLogMessage(this).error(errorMsg);
            }
        }

        return plugin->factory;
    }

    CSimulatorPluginInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorPluginInfoList list;

        std::for_each(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [&list](const PluginData & driver)
        {
            list.push_back(driver.info);
        });

        return list;
    }

    bool CContextSimulator::isConnected() const
    {

        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return false;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->isConnected();
    }

    bool CContextSimulator::canConnect() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return false;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->canConnect();
    }

    bool CContextSimulator::disconnectFromSimulator()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return false;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->disconnectFrom();
    }

    BlackMisc::Simulation::CSimulatorPluginInfo CContextSimulator::getSimulatorPluginInfo() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return BlackMisc::Simulation::CSimulatorPluginInfo(); }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->info;
    }

    CSimulatorSetup CContextSimulator::getSimulatorSetup() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return BlackMisc::Simulation::CSimulatorSetup();
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getSimulatorSetup();
    }

    CAirportList CContextSimulator::getAirportsInRange() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulatorPlugin)
        {
            return CAirportList();
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getAirportsInRange();
    }

    CAircraftModelList CContextSimulator::getInstalledModels() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulatorPlugin)
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getInstalledModels();
    }

    int CContextSimulator::getInstalledModelsCount() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return 0; }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->getInstalledModels().size();
    }

    CAircraftModelList CContextSimulator::getInstalledModelsStartingWith(const QString modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulatorPlugin)
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getInstalledModels().findModelsStartingWith(modelString);
    }

    void CContextSimulator::reloadInstalledModels()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return;
        }
        Q_ASSERT(m_simulatorPlugin->simulator);
        m_simulatorPlugin->simulator->reloadInstalledModels();
    }

    CAircraftIcaoData CContextSimulator::getIcaoForModelString(const QString &modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (!m_simulatorPlugin) { return CAircraftIcaoData(); }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getIcaoForModelString(modelString);
    }

    bool CContextSimulator::setTimeSynchronization(bool enable, CTime offset)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return false; }

        Q_ASSERT(m_simulatorPlugin->simulator);
        bool c = m_simulatorPlugin->simulator->setTimeSynchronization(enable, offset);
        if (!c) { return false; }

        CLogMessage(this).info(enable ? QStringLiteral("Set time syncronization to %1").arg(offset.toQString()) : QStringLiteral("Disabled time syncrhonization"));
        return true;
    }

    bool CContextSimulator::isTimeSynchronized() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return false; }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->isTimeSynchronized();
    }

    int CContextSimulator::getMaxRenderedAircraft() const
    {
        if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return 0;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getMaxRenderedAircraft();
    }

    void CContextSimulator::setMaxRenderedAircraft(int number)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }

        if (m_simulatorPlugin)
        {
            Q_ASSERT(m_simulatorPlugin->simulator);
            m_simulatorPlugin->simulator->setMaxRenderedAircraft(number);
        }
    }

    void CContextSimulator::setMaxRenderedDistance(CLength &distance)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << distance; }
        if (m_simulatorPlugin)
        {
            Q_ASSERT(m_simulatorPlugin->simulator);
            this->m_simulatorPlugin->simulator->setMaxRenderedDistance(distance);
        }
    }

    QString CContextSimulator::getRenderRestrictionText() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return ""; }

        Q_ASSERT(m_simulatorPlugin->simulator);
        if (!m_simulatorPlugin->simulator->isRenderingRestricted()) { return "none"; }
        QString rt;
        if (m_simulatorPlugin->simulator->isMaxAircraftRestricted())
        {
            rt.append(QString::number(m_simulatorPlugin->simulator->getMaxRenderedAircraft())).append(" A/C");
        }
        if (m_simulatorPlugin->simulator->isMaxDistanceRestricted())
        {
            if (!rt.isEmpty()) { rt.append(" ");}
            rt.append(m_simulatorPlugin->simulator->getMaxRenderedDistance().valueRoundedWithUnit(CLengthUnit::NM(), 0));
        }
        return rt;
    }

    CLength CContextSimulator::getMaxRenderedDistance() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return CLength(0, CLengthUnit::nullUnit());
        }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->getMaxRenderedDistance();
    }

    CLength CContextSimulator::getRenderedDistanceBoundary() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin)
        {
            return CLength(20.0, CLengthUnit::NM());
        }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->getRenderedDistanceBoundary();
    }

    void CContextSimulator::deleteAllRenderingRestrictions()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin)
        {
            Q_ASSERT(m_simulatorPlugin->simulator);
            this->m_simulatorPlugin->simulator->deleteAllRenderingRestrictions();
        }
    }

    bool CContextSimulator::isRenderingRestricted() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return false; }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->isRenderingRestricted();
    }

    bool CContextSimulator::isRenderingEnabled() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return false; }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->isRenderingEnabled();
    }

    CTime CContextSimulator::getTimeSynchronizationOffset() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return CTime(0, CTimeUnit::hrmin()); }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return this->m_simulatorPlugin->simulator->getTimeSynchronizationOffset();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        Q_ASSERT(getIContextApplication());
        Q_ASSERT(getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        // warning if we do not have any plugins
        if (m_simulatorPlugins.isEmpty() || simulatorInfo.isUnspecified())
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
        ISimulator *newSimulator = factory->create(simulatorInfo, ownAircraftProvider, renderedAircraftProvider, this);
        Q_ASSERT(newSimulator);

        unloadSimulatorPlugin(); // old plugin unloaded

        PluginData *plugin = findPlugin(simulatorInfo);
        plugin->simulator = newSimulator;
        m_simulatorPlugin = plugin;

        bool c = connect(m_simulatorPlugin->simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::ps_onSimulatorStatusChanged);
        Q_ASSERT(c);
        c = connect(m_simulatorPlugin->simulator, &ISimulator::ownAircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        Q_ASSERT(c);
        c = connect(m_simulatorPlugin->simulator, &ISimulator::modelMatchingCompleted, this, &IContextSimulator::modelMatchingCompleted);
        Q_ASSERT(c);
        c = connect(m_simulatorPlugin->simulator, &ISimulator::installedAircraftModelsChanged, this, &IContextSimulator::installedAircraftModelsChanged);
        Q_ASSERT(c);
        c = connect(m_simulatorPlugin->simulator, &ISimulator::renderRestrictionsChanged, this, &IContextSimulator::renderRestrictionsChanged);
        Q_ASSERT(c);
        c = connect(m_simulatorPlugin->simulator, &ISimulator::airspaceSnapshotHandled, this, &IContextSimulator::airspaceSnapshotHandled);
        Q_ASSERT(c);

        // log from context to simulator
        c = connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, m_simulatorPlugin->simulator, &ISimulator::displayStatusMessage);
        Q_ASSERT(c);
        c = connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, m_simulatorPlugin->simulator, &ISimulator::displayStatusMessage);
        Q_ASSERT(c);
        Q_UNUSED(c);

        // connect with network
        IContextNetwork *networkContext = this->getIContextNetwork();
        Q_ASSERT(networkContext);
        Q_ASSERT(networkContext->isLocalObject());

        for (const CSimulatedAircraft &simulatedAircraft : networkContext->getAircraftInRange())
        {
            Q_ASSERT(!simulatedAircraft.getCallsign().isEmpty());
            m_simulatorPlugin->simulator->logicallyAddRemoteAircraft(simulatedAircraft);
        }

        // apply latest settings
        settingsChanged(static_cast<uint>(IContextSettings::SettingsSimulator));

        // try to connect
        m_simulatorPlugin->simulator->asyncConnectTo();

        if (m_simulatorPlugin) // can be already nullptr if connectTo() is synchronous and fails
        {
            emit simulatorPluginChanged(this->m_simulatorPlugin->info);
            CLogMessage(this).info("Simulator plugin loaded: %1") << this->m_simulatorPlugin->info.toQString(true);
            return true;
        }
        else
        {
            return false;
        }
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

        if (this->m_simulatorPlugin)
        {
            // already loaded
            CLogMessage(this).warning("Cannot listen for simulator while the driver %1 is still loaded") << m_simulatorPlugin->info.toQString();
            return;
        }

        // warning if we do not have any plugins
        if (m_simulatorPlugins.isEmpty())
        {
            CLogMessage(this).error("No simulator drivers available");
            return;
        }

        PluginData *plugin = findPlugin(simulatorInfo);
        if (!plugin)
        {
            CLogMessage(this).error("Driver not found for '%1'") << simulatorInfo.toQString();
            return;
        }

        if (!plugin->listener)
        {
            ISimulatorFactory *factory = getSimulatorFactory(simulatorInfo);
            Q_ASSERT(factory);

            plugin->listener = factory->createListener();
            bool c = connect(plugin->listener, &ISimulatorListener::simulatorStarted,
                             m_mapper, static_cast<void (QSignalMapper::*)()> (&QSignalMapper::map));
            if (!c)
            {
                CLogMessage(this).error("Unable to use '%1'") << simulatorInfo.toQString();
                return;
            }
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
        CLogMessage(this).debug() << "Listening for simulator: " << simulatorInfo.toQString(true);
    }

    void CContextSimulator::listenForAllSimulators()
    {
        auto plugins = getAvailableSimulatorPlugins();
        for (const auto &p : plugins)
        {
            listenForSimulator(p);
        }
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
        if (m_simulatorPlugin)
        {
            // depending on shutdown order, network might already have been deleted
            emit simulatorPluginChanged(CSimulatorPluginInfo());

            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextNetwork()->isLocalObject());
            Q_ASSERT(m_simulatorPlugin->simulator);

            // disconnect from simulator
            if (m_simulatorPlugin->simulator->isConnected())
            {
                m_simulatorPlugin->simulator->disconnectFrom();
            }

            // disconnect signals
            this->getRuntime()->getCContextNetwork()->disconnectRemoteAircraftProviderSignals(m_simulatorPlugin->simulator);
            m_simulatorPlugin->simulator->disconnect();
            CLogHandler::instance()->disconnect(m_simulatorPlugin->simulator);
            this->disconnect(m_simulatorPlugin->simulator);

            m_simulatorPlugin->simulator->deleteLater();
            m_simulatorPlugin->simulator = nullptr;
            m_simulatorPlugin = nullptr;
        }
    }

    void CContextSimulator::ps_addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (!m_simulatorPlugin)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        Q_ASSERT(!remoteAircraft.getCallsign().isEmpty());

        m_simulatorPlugin->simulator->logicallyAddRemoteAircraft(remoteAircraft);
    }

    void CContextSimulator::ps_removedRemoteAircraft(const CCallsign &callsign)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (!m_simulatorPlugin)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        m_simulatorPlugin->simulator->logicallyRemoveRemoteAircraft(callsign);
    }

    void CContextSimulator::ps_onSimulatorStatusChanged(int status)
    {
        Q_ASSERT(m_simulatorPlugin);
        Q_ASSERT(m_simulatorPlugin->simulator);

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
        if (!m_simulatorPlugin)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        for (const auto &tm : textMessages)
        {
            m_simulatorPlugin->simulator->displayTextMessage(tm);
        }
    }

    void CContextSimulator::ps_cockpitChangedFromSimulator(const CSimulatedAircraft &ownAircraft)
    {
        Q_ASSERT(getIContextOwnAircraft());
        getIContextOwnAircraft()->changedAircraftCockpit(ownAircraft, IContextSimulator::InterfaceName());
    }

    void CContextSimulator::ps_changedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulatorPlugin);
        this->m_simulatorPlugin->simulator->changeRemoteAircraftModel(aircraft, originator);
    }

    void CContextSimulator::ps_changedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulatorPlugin);
        this->m_simulatorPlugin->simulator->changeRemoteAircraftEnabled(aircraft, originator);
    }

    void CContextSimulator::ps_updateSimulatorCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (!m_simulatorPlugin)
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);

        // avoid loops
        if (originator.isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

        // update
        this->m_simulatorPlugin->simulator->updateOwnSimulatorCockpit(ownAircraft, originator);
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
        if (!m_simulatorPlugin)
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
            m_simulatorPlugin->simulator->setTimeSynchronization(timeSync, syncOffset);
        }
    }

    CPixmap CContextSimulator::iconForModel(const QString &modelString) const
    {
        if (!this->m_simulatorPlugin)
        {
            return CPixmap();
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->iconForModel(modelString);
    }

    void CContextSimulator::enableDebugMessages(bool driver, bool interpolator)
    {
        if (!this->m_simulatorPlugin)
        {
            return;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->enableDebugMessages(driver, interpolator);
    }

    void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        Q_ASSERT(m_simulatorPlugin->simulator);
        this->m_simulatorPlugin->simulator->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
    }

    bool CContextSimulator::isPaused() const
    {
        if (!this->m_simulatorPlugin)
        {
            return false;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->isPaused();
    }

    bool CContextSimulator::isSimulating() const
    {
        if (!this->m_simulatorPlugin)
        {
            return false;
        }

        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->isSimulating();
    }

    void CContextSimulator::ps_simulatorStarted(QObject *listener)
    {
        Q_ASSERT(listener);
        Q_ASSERT(listener->inherits("BlackCore::ISimulatorListener"));

        /* Find caller */
        PluginData *plugin = [this](QObject * listener)
        {
            auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [listener](const PluginData & plugin)
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
        for (const auto &fileName : fileNames)
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
                m_simulatorPlugins << PluginData { simulatorPluginInfo, nullptr, nullptr, nullptr, pluginPath};
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
        std::for_each(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [](PluginData & plugin)
        {
            if (plugin.listener)
                QMetaObject::invokeMethod(plugin.listener, "stop");
        });
    }

    CContextSimulator::PluginData *CContextSimulator::findPlugin(const CSimulatorPluginInfo &info)
    {
        auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [&info](PluginData & plugin)
        {
            return plugin.info == info;
        });

        return &(*it);
    }

} // namespace
