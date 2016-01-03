/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextsimulatorimpl.h"
#include "contextownaircraftimpl.h"
#include "contextownaircraft.h"
#include "contextapplication.h"
#include "contextnetworkimpl.h"
#include "pluginmanagersimulator.h"
#include "corefacade.h"
#include "blackcore/registermetadata.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/threadutils.h"
#include <QPluginLoader>
#include <QLibrary>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
        IContextSimulator(mode, runtime),
        m_plugins(new CPluginManagerSimulator(this))
    {
        this->setObjectName("CContextSimulator");
        m_plugins->collectPlugins();
    }

    CContextSimulator *CContextSimulator::registerWithDBus(CDBusServer *server)
    {
        if (!server || this->m_mode != CCoreFacadeConfig::LocalInDbusServer) return this;
        server->addObject(CContextSimulator::ObjectPath(), this);
        return this;
    }

    CContextSimulator::~CContextSimulator()
    {
        this->gracefulShutdown();
    }

    void CContextSimulator::gracefulShutdown()
    {
        this->disconnect();
        this->unloadSimulatorPlugin();
    }

    CSimulatorPluginInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        return m_plugins->getAvailableSimulatorPlugins();
    }

    bool CContextSimulator::startSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        return this->loadSimulatorPlugin(simulatorInfo, true);
    }

    void CContextSimulator::stopSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
    {
        if (!m_simulatorPlugin.first.isUnspecified() && m_simulatorPlugin.first == simulatorInfo)
        {
            this->unloadSimulatorPlugin();
        }

        ISimulatorListener *listener = m_plugins->getListener(simulatorInfo.getIdentifier());
        Q_ASSERT(listener);
        QMetaObject::invokeMethod(listener, "stop");
    }

    int CContextSimulator::getSimulatorStatus() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return 0; }

        Q_ASSERT_X(m_simulatorPlugin.second, Q_FUNC_INFO, "Missing simulator");
        return m_simulatorPlugin.second->getSimulatorStatus();
    }

    BlackMisc::Simulation::CSimulatorPluginInfo CContextSimulator::getSimulatorPluginInfo() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return BlackMisc::Simulation::CSimulatorPluginInfo(); }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.first;
    }

    CSimulatorSetup CContextSimulator::getSimulatorSetup() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return BlackMisc::Simulation::CSimulatorSetup();
        }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getSimulatorSetup();
    }

    CAirportList CContextSimulator::getAirportsInRange() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return CAirportList();
        }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getAirportsInRange();
    }

    CAircraftModelList CContextSimulator::getInstalledModels() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getInstalledModels();
    }

    int CContextSimulator::getInstalledModelsCount() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return 0; }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getInstalledModels().size();
    }

    CAircraftModelList CContextSimulator::getInstalledModelsStartingWith(const QString modelString) const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return CAircraftModelList();
        }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getInstalledModels().findModelsStartingWith(modelString);
    }

    void CContextSimulator::reloadInstalledModels()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return;
        }
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->reloadInstalledModels();
    }

    bool CContextSimulator::setTimeSynchronization(bool enable, const CTime &offset)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return false; }

        Q_ASSERT(m_simulatorPlugin.second);
        bool c = m_simulatorPlugin.second->setTimeSynchronization(enable, offset);
        if (!c) { return false; }

        CLogMessage(this).info(enable ? QStringLiteral("Set time syncronization to %1").arg(offset.toQString()) : QStringLiteral("Disabled time syncrhonization"));
        return true;
    }

    bool CContextSimulator::isTimeSynchronized() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return false; }

        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->isTimeSynchronized();
    }

    int CContextSimulator::getMaxRenderedAircraft() const
    {
        if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return 0; }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getMaxRenderedAircraft();
    }

    void CContextSimulator::setMaxRenderedAircraft(int number)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }
        if (m_simulatorPlugin.first.isUnspecified()) { return; }
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->setMaxRenderedAircraft(number);

    }

    void CContextSimulator::setMaxRenderedDistance(const CLength &distance)
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << distance; }
        if (m_simulatorPlugin.first.isUnspecified()) { return; }
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->setMaxRenderedDistance(distance);
    }

    QString CContextSimulator::getRenderRestrictionText() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return ""; }

        Q_ASSERT(m_simulatorPlugin.second);
        if (!m_simulatorPlugin.second->isRenderingRestricted()) { return "none"; }
        QString rt;
        if (m_simulatorPlugin.second->isMaxAircraftRestricted())
        {
            rt.append(QString::number(m_simulatorPlugin.second->getMaxRenderedAircraft())).append(" A/C");
        }
        if (m_simulatorPlugin.second->isMaxDistanceRestricted())
        {
            if (!rt.isEmpty()) { rt.append(" ");}
            rt.append(m_simulatorPlugin.second->getMaxRenderedDistance().valueRoundedWithUnit(CLengthUnit::NM(), 0));
        }
        return rt;
    }

    CLength CContextSimulator::getMaxRenderedDistance() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return CLength(0, CLengthUnit::nullUnit()); }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getMaxRenderedDistance();
    }

    CLength CContextSimulator::getRenderedDistanceBoundary() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified())
        {
            return CLength(20.0, CLengthUnit::NM());
        }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getRenderedDistanceBoundary();
    }

    void CContextSimulator::deleteAllRenderingRestrictions()
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.first.isUnspecified())
        {
            Q_ASSERT(m_simulatorPlugin.second);
            m_simulatorPlugin.second->deleteAllRenderingRestrictions();
        }
    }

    bool CContextSimulator::isRenderingRestricted() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return false; }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->isRenderingRestricted();
    }

    bool CContextSimulator::isRenderingEnabled() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return false; }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->isRenderingEnabled();
    }

    CTime CContextSimulator::getTimeSynchronizationOffset() const
    {
        if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.first.isUnspecified()) { return CTime(0, CTimeUnit::hrmin()); }
        Q_ASSERT(m_simulatorPlugin.second);
        return m_simulatorPlugin.second->getTimeSynchronizationOffset();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo, bool withListener)
    {
        Q_ASSERT(getIContextApplication());
        Q_ASSERT(getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());
        Q_ASSERT(CThreadUtils::isCurrentThreadApplicationThread()); // only run in main thread

        if (!simulatorInfo.isValid())
        {
            CLogMessage(this).error("Illegal plugin");
            return false;
        }

        // Is the plugin already loaded?
        if (!m_simulatorPlugin.first.isUnspecified())
        {
            return true;
        }

        unloadSimulatorPlugin(); // old plugin unloaded

        // now we have a state where no driver is loaded
        if (withListener)
        {
            this->listenForSimulator(simulatorInfo);
            return false; // not a plugin yet, just listener
        }

        if (!simulatorInfo.isValid() || simulatorInfo.isUnspecified())
        {
            CLogMessage(this).error("Illegal plugin");
            return false;
        }

        ISimulatorFactory *factory = m_plugins->getFactory(simulatorInfo.getIdentifier());
        Q_ASSERT_X(factory, Q_FUNC_INFO, "no factory");

        // We assume we run in the same process as the own aircraft context
        // Hence we pass in memory reference to own aircraft object
        Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());
        Q_ASSERT(this->getIContextNetwork()->isUsingImplementingObject());
        IOwnAircraftProvider *ownAircraftProvider = this->getRuntime()->getCContextOwnAircraft();
        IRemoteAircraftProvider *renderedAircraftProvider = this->getRuntime()->getCContextNetwork();
        ISimulator *simulator = factory->create(simulatorInfo, ownAircraftProvider, renderedAircraftProvider, m_plugins, &m_weatherManager);
        Q_ASSERT_X(simulator, Q_FUNC_INFO, "no simulator driver can be created");

        bool c = connect(simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::ps_onSimulatorStatusChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::ownAircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::modelMatchingCompleted, this, &IContextSimulator::modelMatchingCompleted);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::installedAircraftModelsChanged, this, &IContextSimulator::installedAircraftModelsChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::renderRestrictionsChanged, this, &IContextSimulator::renderRestrictionsChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::airspaceSnapshotHandled, this, &IContextSimulator::airspaceSnapshotHandled);
        Q_ASSERT(c);

        // log from context to simulator
        c = connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, simulator, &ISimulator::displayStatusMessage);
        Q_ASSERT(c);
        c = connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, simulator, &ISimulator::displayStatusMessage);
        Q_ASSERT(c);
        Q_UNUSED(c);

        // use network to initally add aircraft
        IContextNetwork *networkContext = this->getIContextNetwork();
        Q_ASSERT(networkContext);
        Q_ASSERT(networkContext->isLocalObject());

        // initially add aircraft
        for (const CSimulatedAircraft &simulatedAircraft : networkContext->getAircraftInRange())
        {
            Q_ASSERT(!simulatedAircraft.getCallsign().isEmpty());
            simulator->logicallyAddRemoteAircraft(simulatedAircraft);
        }

        // when everything is set up connected, update the current plugin info
        m_simulatorPlugin.first = simulatorInfo;
        m_simulatorPlugin.second = simulator;

        // try to connect to simulator
        simulator->connectTo();
        emit simulatorPluginChanged(simulatorInfo);
        CLogMessage(this).info("Simulator plugin loaded: %1") << simulatorInfo.toQString(true);

        return true;
    }

    void CContextSimulator::listenForSimulator(const CSimulatorPluginInfo &simulatorInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorInfo.isUnspecified());

        ISimulator::SimulatorStatus simStatus = getSimulatorStatusEnum();
        if (!m_simulatorPlugin.first.isUnspecified() &&
                m_simulatorPlugin.first == simulatorInfo && simStatus.testFlag(ISimulator::Connected))
        {
            // the simulator is already connected and running
            return;
        }

        if (!m_simulatorPlugin.first.isUnspecified())
        {
            // wrong or disconnected plugin, we start from the scratch
            this->unloadSimulatorPlugin();
        }

        if (!m_listenersThread.isRunning())
        {
            m_listenersThread.setObjectName("CContextSimulator:Thread for listeners");
            m_listenersThread.start(QThread::LowPriority);
        }

        ISimulatorListener *listener = m_plugins->createListener(simulatorInfo.getIdentifier());
        Q_ASSERT_X(listener, Q_FUNC_INFO, "No listener");

        if (listener->thread() != &m_listenersThread)
        {
            bool c = connect(listener, &ISimulatorListener::simulatorStarted, this, &CContextSimulator::ps_simulatorStarted);
            if (!c)
            {
                CLogMessage(this).error("Unable to use '%1'") << simulatorInfo.toQString();
                return;
            }

            Q_ASSERT_X(!listener->parent(), Q_FUNC_INFO, "Objects with parent cannot be moved to thread");
            listener->setProperty("isInitialized", true);
            listener->moveToThread(&m_listenersThread);
        }

        bool s = QMetaObject::invokeMethod(listener, "start", Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke method");
        Q_UNUSED(s);

        CLogMessage(this).info("Listening for simulator %1") << simulatorInfo.getIdentifier();
    }

    void CContextSimulator::listenForAllSimulators()
    {
        auto plugins = getAvailableSimulatorPlugins();
        for (const CSimulatorPluginInfo &p : plugins)
        {
            Q_ASSERT(!p.isUnspecified());

            if (p.isValid())
            {
                listenForSimulator(p);
            }
        }
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (!m_simulatorPlugin.first.isUnspecified())
        {
            ISimulator *sim = m_simulatorPlugin.second;
            m_simulatorPlugin.second = nullptr;
            m_simulatorPlugin.first = CSimulatorPluginInfo();

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
    }

    void CContextSimulator::ps_addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        //! \todo This was previously an assert and it should be one again in the future. This slot should not even be called when no simulator is available.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin.second);
        Q_ASSERT(!remoteAircraft.getCallsign().isEmpty());

        m_simulatorPlugin.second->logicallyAddRemoteAircraft(remoteAircraft);
    }

    void CContextSimulator::ps_removedRemoteAircraft(const CCallsign &callsign)
    {
        // \fixme: This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(callsign);
    }

    void CContextSimulator::ps_onSimulatorStatusChanged(int status)
    {
        ISimulator::SimulatorStatus statusEnum = ISimulator::statusToEnum(status);
        if (!statusEnum.testFlag(ISimulator::Connected))
        {
            // we got disconnected, plugin no longer needed
            unloadSimulatorPlugin();
        }
        emit simulatorStatusChanged(status);
    }

    void CContextSimulator::ps_textMessagesReceived(const Network::CTextMessageList &textMessages)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin.second);
        for (const auto &tm : textMessages)
        {
            m_simulatorPlugin.second->displayTextMessage(tm);
        }
    }

    void CContextSimulator::ps_cockpitChangedFromSimulator(const CSimulatedAircraft &ownAircraft)
    {
        Q_ASSERT(getIContextOwnAircraft());
        getIContextOwnAircraft()->changedAircraftCockpit(ownAircraft, IContextSimulator::InterfaceName());
    }

    void CContextSimulator::ps_changedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
    {
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->changeRemoteAircraftModel(aircraft, originator);
    }

    void CContextSimulator::ps_changedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
    {
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->changeRemoteAircraftEnabled(aircraft, originator);
    }

    void CContextSimulator::ps_updateSimulatorCockpitFromContext(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
    {
        // todo:
        // This was previously an assert and it should be one again in the future.
        // This slot should not even be called when no simulator is available.
        if (m_simulatorPlugin.first.isUnspecified())
        {
            // Do something if no simulator is running
            return;
        }

        Q_ASSERT(m_simulatorPlugin.second);

        // avoid loops
        if (originator.getName().isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

        // update
        m_simulatorPlugin.second->updateOwnSimulatorCockpit(ownAircraft, originator);
    }

    void CContextSimulator::restoreSimulatorPlugins()
    {
        stopSimulatorListeners();

        auto enabledSimulators = m_enabledSimulators.get();
        auto allSimulators = m_plugins->getAvailableSimulatorPlugins();
        for (const CSimulatorPluginInfo& s: allSimulators)
        {
            if (enabledSimulators.contains(s.getIdentifier()))
            {
                startSimulatorPlugin(s);
            }
        }
    }

    CPixmap CContextSimulator::iconForModel(const QString &modelString) const
    {
        if (m_simulatorPlugin.first.isUnspecified()) { return CPixmap(); }
        Q_ASSERT_X(m_simulatorPlugin.second, Q_FUNC_INFO, "Missing simulator");
        return m_simulatorPlugin.second->iconForModel(modelString);
    }

    void CContextSimulator::enableDebugMessages(bool driver, bool interpolator)
    {
        if (m_simulatorPlugin.first.isUnspecified()) { return; }
        Q_ASSERT_X(m_simulatorPlugin.second, Q_FUNC_INFO, "Missing simulator");
        return m_simulatorPlugin.second->enableDebugMessages(driver, interpolator);
    }

    void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        Q_ASSERT(m_simulatorPlugin.second);
        m_simulatorPlugin.second->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
    }

    void CContextSimulator::ps_simulatorStarted(const CSimulatorPluginInfo &info)
    {
        stopSimulatorListeners();
        loadSimulatorPlugin(info, false);
    }

    void CContextSimulator::stopSimulatorListeners()
    {
        for (const auto &info : getAvailableSimulatorPlugins())
        {
            ISimulatorListener *listener = m_plugins->getListener(info.getIdentifier());
            if(listener) { QMetaObject::invokeMethod(listener, "stop"); }
        }
    }

} // namespace
