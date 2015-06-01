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
#include "blackmisc/settingutilities.h"
#include <QPluginLoader>
#include <QLibrary>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
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
        this->setObjectName("CContextSimulator");
        findSimulatorPlugins();
        // Maps listener instance
        connect(m_mapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped), this, &CContextSimulator::ps_simulatorStarted);
    }

    CContextSimulator::~CContextSimulator()
    {
        this->gracefulShutdown();
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

    CVariant CContextSimulator::getPluginData(const QObject *obj, const QString &key) const
    {
        const QObject *p = obj;
        while (p && !p->inherits("BlackCore::ISimulatorFactory"))
        {
            p = p->parent();
        }

        if (!p) return CVariant();
        auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [p](const PluginData & plugin)
        {
            return plugin.factory == qobject_cast<ISimulatorFactory *>(p);
        });
        Q_ASSERT(it != m_simulatorPlugins.end());
        return it->m_storage.value(key);
    }

    void CContextSimulator::setPluginData(const QObject *obj, const QString &key, const CVariant &value)
    {
        const QObject *p = obj;
        while (p && !p->inherits("BlackCore::ISimulatorFactory"))
        {
            p = p->parent();
        }

        if (!p) { return; }
        auto it = std::find_if(m_simulatorPlugins.begin(), m_simulatorPlugins.end(), [p](const PluginData & plugin)
        {
            return plugin.factory == qobject_cast<ISimulatorFactory *>(p);
        });
        Q_ASSERT(it != m_simulatorPlugins.end());
        it->m_storage.insert(key, value);
    }

    void CContextSimulator::gracefulShutdown()
    {
        this->disconnect();
        this->stopSimulatorPlugin();
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

    bool CContextSimulator::startSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        return this->loadSimulatorPlugin(simulatorInfo, true);
    }

    void CContextSimulator::stopSimulatorPlugin()
    {
        this->unloadSimulatorPlugin(false);
    }

    int CContextSimulator::getSimulatorStatus() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin) { return 0; }

        Q_ASSERT_X(m_simulatorPlugin->simulator, Q_FUNC_INFO, "Missing simulator");
        return m_simulatorPlugin->simulator->getSimulatorStatus();
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

    bool CContextSimulator::setTimeSynchronization(bool enable, const CTime &offset)
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
        if (!m_simulatorPlugin) { return 0; }
        Q_ASSERT(m_simulatorPlugin->simulator);
        return m_simulatorPlugin->simulator->getMaxRenderedAircraft();
    }

    void CContextSimulator::setMaxRenderedAircraft(int number)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }
        if (!m_simulatorPlugin) { return; }
        Q_ASSERT(m_simulatorPlugin->simulator);
        m_simulatorPlugin->simulator->setMaxRenderedAircraft(number);

    }

    void CContextSimulator::setMaxRenderedDistance(const CLength &distance)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << distance; }
        if (!m_simulatorPlugin) { return; }
        Q_ASSERT(m_simulatorPlugin->simulator);
        this->m_simulatorPlugin->simulator->setMaxRenderedDistance(distance);
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
        if (!m_simulatorPlugin) { return CLength(0, CLengthUnit::nullUnit()); }
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

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo, bool withListener)
    {
        Q_ASSERT(getIContextApplication());
        Q_ASSERT(getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        // error if we do not have any plugins
        if (m_simulatorPlugins.isEmpty())
        {
            CLogMessage(this).error("No simulator plugins");
            return false;
        }

        // Is the plugin already loaded?
        if (m_simulatorPlugin && m_simulatorPlugin->info == simulatorInfo)
        {
            return true;
        }

        unloadSimulatorPlugin(false); // old plugin unloaded

        // now we have a state where no driver is loaded
        if (withListener)
        {
            // hand over to listeners, when listener is done, it will call this function again
            if (simulatorInfo.isAuto())
            {
                this->listenForAllSimulators();
            }
            else
            {
                this->listenForSimulator(simulatorInfo);
            }
            return false; // not a plugin yet, just listener
        }

        if (!simulatorInfo.isValid() || simulatorInfo.isUnspecified())
        {
            CLogMessage(this).error("Illegal plugin");
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
        Q_ASSERT_X(newSimulator, Q_FUNC_INFO, "no simulator driver can be created");

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

        // initially add aircraft
        for (const CSimulatedAircraft &simulatedAircraft : networkContext->getAircraftInRange())
        {
            Q_ASSERT(!simulatedAircraft.getCallsign().isEmpty());
            m_simulatorPlugin->simulator->logicallyAddRemoteAircraft(simulatedAircraft);
        }

        // try to connect
        m_simulatorPlugin->simulator->connectTo();
        emit simulatorPluginChanged(this->m_simulatorPlugin->info);
        CLogMessage(this).info("Simulator plugin loaded: %1") << this->m_simulatorPlugin->info.toQString(true);

        return true;
    }

    void CContextSimulator::listenForSimulator(const CSimulatorPluginInfo &simulatorInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        // warning if we do not have any plugins
        if (m_simulatorPlugins.isEmpty())
        {
            CLogMessage(this).error("No simulator drivers available");
            return;
        }

        ISimulator::SimulatorStatus simStatus = getSimulatorStatusEnum();
        if (this->m_simulatorPlugin && this->m_simulatorPlugin->info == simulatorInfo && simStatus.testFlag(ISimulator::Connected))
        {
            // the simulator is already connected and running
            return;
        }

        if (this->m_simulatorPlugin)
        {
            // wrong or disconnected plugin, we start from the scratch
            this->unloadSimulatorPlugin(false);
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
        Q_ASSERT_X(listener, Q_FUNC_INFO, "No listener");

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
        for (const CSimulatorPluginInfo &p : plugins)
        {
            if (p.isUnspecified()) { continue; }
            if (p.isValid())
            {
                listenForSimulator(p);
            }
        }
    }

    void CContextSimulator::unloadSimulatorPlugin(bool startListeners)
    {
        if (m_simulatorPlugin)
        {
            ISimulator *sim = this->m_simulatorPlugin->simulator;
            m_simulatorPlugin = nullptr;

            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextNetwork()->isLocalObject());

            // unload and disconnect
            if (sim)
            {
                // disconnect signals and delete
                sim->unload();
                this->disconnect(sim);
                sim->deleteLater();
                emit simulatorPluginChanged(CSimulatorPluginInfo());
            }
        }

        if (startListeners)
        {
            this->listenForAllSimulators();
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
        ISimulator::SimulatorStatus statusEnum = ISimulator::statusToEnum(status);
        if (!statusEnum.testFlag(ISimulator::Connected))
        {
            // we got disconnected, plugin no longer needed
            unloadSimulatorPlugin(false);

            // do not immediately listen again, but allow some time for the simulator to shutdown
            // otherwise we risk reconnecting to a closing simulator
            BlackMisc::singleShot(1000, QThread::currentThread(), [ = ]()
            {
                listenForAllSimulators();
            });
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

    void CContextSimulator::ps_changedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const COriginator &originator)
    {
        Q_ASSERT(this->m_simulatorPlugin);
        this->m_simulatorPlugin->simulator->changeRemoteAircraftModel(aircraft, originator);
    }

    void CContextSimulator::ps_changedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const COriginator &originator)
    {
        Q_ASSERT(this->m_simulatorPlugin);
        this->m_simulatorPlugin->simulator->changeRemoteAircraftEnabled(aircraft, originator);
    }

    void CContextSimulator::ps_updateSimulatorCockpitFromContext(const CAircraft &ownAircraft, const COriginator &originator)
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
        if (originator.getName().isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

        // update
        this->m_simulatorPlugin->simulator->updateOwnSimulatorCockpit(ownAircraft, originator);
    }

    void CContextSimulator::settingsChanged(uint type)
    {
        auto settingsType = static_cast<IContextSettings::SettingsType>(type);
        if (settingsType != IContextSettings::SettingsSimulator) { return; }

        // simulator code would go here
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
        loadSimulatorPlugin(plugin->info, false);
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
            {
                QMetaObject::invokeMethod(plugin.listener, "stop");
            }
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
