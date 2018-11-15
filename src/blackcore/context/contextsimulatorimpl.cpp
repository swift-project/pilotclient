/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/corefacade.h"
#include "blackcore/application.h"
#include "blackcore/pluginmanagersimulator.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"
#include "contextsimulatorimpl.h"

#include <QMetaObject>
#include <QStringList>
#include <QThread>
#include <Qt>
#include <QtGlobal>
#include <QPointer>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::XPlane;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::Data;

namespace BlackCore
{
    namespace Context
    {
        CContextSimulator::CContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextSimulator(mode, runtime),
            CIdentifiable(this),
            m_plugins(new CPluginManagerSimulator(this))
        {
            this->setObjectName("CContextSimulator");
            CContextSimulator::registerHelp();

            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");
            m_enableMatchingMessages = CBuildConfig::isLocalDeveloperDebugBuild(); // can be slow with huge model sets
            m_plugins->collectPlugins();
            this->restoreSimulatorPlugins();

            connect(&m_weatherManager, &CWeatherManager::weatherGridReceived, this, &CContextSimulator::weatherGridReceived);
            connect(&m_aircraftMatcher, &CAircraftMatcher::setupChanged, this, &CContextSimulator::matchingSetupChanged);
            connect(&CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance(), &CCentralMultiSimulatorModelSetCachesProvider::cacheChanged, this, &CContextSimulator::modelSetChanged);

            // deferred init of last model set, if no other data are set in meantime
            const QPointer<CContextSimulator> myself(this);
            QTimer::singleShot(2500, this, [ = ]
            {
                if (!myself) { return; }
                this->initByLastUsedModelSet();
                m_aircraftMatcher.setSetup(m_matchingSettings.get());
                if (m_aircraftMatcher.getModelSetCount() <= MatchingLogMaxModelSetSize)
                {
                    this->enableMatchingMessages(true);
                }
            });
        }

        CContextSimulator *CContextSimulator::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
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
            return this->listenForSimulator(simulatorInfo);
        }

        void CContextSimulator::stopSimulatorPlugin(const CSimulatorPluginInfo &simulatorInfo)
        {
            if (!m_simulatorPlugin.first.isUnspecified() && m_simulatorPlugin.first == simulatorInfo)
            {
                this->unloadSimulatorPlugin();
            }

            ISimulatorListener *listener = m_plugins->getListener(simulatorInfo.getIdentifier());
            Q_ASSERT(listener);
            QMetaObject::invokeMethod(listener, "stop", Qt::QueuedConnection);
        }

        int CContextSimulator::checkListeners()
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_plugins) { return 0; }
            return m_plugins->checkAvailableListeners();
        }

        int CContextSimulator::getSimulatorStatus() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return 0; }
            return m_simulatorPlugin.second->getSimulatorStatus();
        }

        CSimulatorPluginInfo CContextSimulator::getSimulatorPluginInfo() const
        {
            static const CSimulatorPluginInfo unspecified;
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return unspecified; }
            if (m_simulatorPlugin.first.getSimulator().contains("emulated", Qt::CaseInsensitive)) { return m_simulatorPlugin.second->getSimulatorPluginInfo(); }
            return m_simulatorPlugin.first;
        }

        CSimulatorInternals CContextSimulator::getSimulatorInternals() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CSimulatorInternals(); }
            return m_simulatorPlugin.second->getSimulatorInternals();
        }

        CAirportList CContextSimulator::getAirportsInRange(bool recalculateDistance) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            // If no ISimulator object is available, return a dummy.
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CAirportList(); }
            return m_simulatorPlugin.second->getAirportsInRange(recalculateDistance);
        }

        CAircraftModelList CContextSimulator::getModelSet() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const CSimulatorInfo simulator = m_modelSetSimulator.get();
            if (!simulator.isSingleSimulator()) { return CAircraftModelList(); }

            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
        }

        CSimulatorInfo CContextSimulator::getModelSetLoaderSimulator() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_modelSetSimulator.get();
        }

        void CContextSimulator::setModelSetLoaderSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (this->isSimulatorAvailable()) { return; }
            m_modelSetSimulator.set(simulator);
            const CAircraftModelList models = this->getModelSet();
            m_aircraftMatcher.setModelSet(models, simulator, false);
        }

        CSimulatorInfo CContextSimulator::simulatorsWithInitializedModelSet() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().simulatorsWithInitializedCache();
        }

        CStatusMessageList CContextSimulator::verifyPrerequisites() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CStatusMessageList msgs;
            if (!sApp || !sApp->isNetworkAccessible())
            {
                msgs.push_back(CStatusMessage(this).error("No network interface, simulation will not work properly"));
            }
            const CSimulatorInfo simulators = this->simulatorsWithInitializedModelSet();
            if (simulators.isNoSimulator())
            {
                msgs.push_back(CStatusMessage(this).error("No model set so far, you need at least one model set. Hint: You can create a model set in the mapping tool, or copy an existing set in the launcher."));
            }
            else if (simulators.isXPlane() || CSimulatorInfo(m_enabledSimulators.get()).isXPlane())
            {
                // ever used with XPlane
                const QString pluginDir = CXPlaneUtil::pluginDirFromRootDir(m_simulatorSettings.getSimulatorDirectoryOrDefault(CSimulatorInfo::XPLANE));
                if (CDirectoryUtils::isDirExisting(pluginDir))
                {
                    // only check if we are on a XP machine
                    const QStringList conflicts = CXPlaneUtil::findConflictingPlugins(pluginDir);
                    if (!conflicts.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).warning("Possible conflict with other XPlane plugins: '%1'") << (conflicts.join(", ")));
                    }
                }
            }
            return msgs;
        }

        QStringList CContextSimulator::getModelSetStrings() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return this->getModelSet().getModelStringList(false);
        }

        bool CContextSimulator::isKnownModel(const QString &modelString) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const bool known = this->getModelSet().containsModelString(modelString);
            return known;
        }

        QStringList CContextSimulator::getModelSetCompleterStrings(bool sorted) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << sorted; }
            return this->getModelSet().toCompleterStrings(sorted);
        }

        int CContextSimulator::getModelSetCount() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return 0; }
            return this->getModelSet().size();
        }

        CAircraftModelList CContextSimulator::getModelSetModelsStartingWith(const QString &modelString) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CAircraftModelList(); }

            return this->getModelSet().findModelsStartingWith(modelString);
        }

        bool CContextSimulator::setTimeSynchronization(bool enable, const CTime &offset)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return false; }

            const bool c = m_simulatorPlugin.second->setTimeSynchronization(enable, offset);
            if (!c) { return false; }

            CLogMessage(this).info(enable ? QStringLiteral("Set time syncronization to %1").arg(offset.toQString()) : QStringLiteral("Disabled time syncrhonization"));
            return true;
        }

        bool CContextSimulator::isTimeSynchronized() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return false; }
            return m_simulatorPlugin.second->isTimeSynchronized();
        }

        CInterpolationAndRenderingSetupGlobal CContextSimulator::getInterpolationAndRenderingSetupGlobal() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return m_renderSettings.get(); }
            return m_simulatorPlugin.second->getInterpolationSetupGlobal();
        }

        CInterpolationSetupList CContextSimulator::getInterpolationAndRenderingSetupsPerCallsign() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CInterpolationSetupList(); }
            return m_simulatorPlugin.second->getInterpolationSetupsPerCallsign();
        }

        CInterpolationAndRenderingSetupPerCallsign CContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault(const CCallsign &callsign) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CInterpolationAndRenderingSetupPerCallsign(); }
            return m_simulatorPlugin.second->getInterpolationSetupPerCallsignOrDefault(callsign);
        }

        bool CContextSimulator::setInterpolationAndRenderingSetupsPerCallsign(const CInterpolationSetupList &setups, bool ignoreSameAsGlobal)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return false; }
            return m_simulatorPlugin.second->setInterpolationSetupsPerCallsign(setups, ignoreSameAsGlobal);
        }

        void CContextSimulator::setInterpolationAndRenderingSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << setup; }

            // anyway save for future reference
            const CStatusMessage m = m_renderSettings.setAndSave(setup);
            CLogMessage::preformatted(m);

            // transfer to sim
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return; }
            m_simulatorPlugin.second->setInterpolationSetupGlobal(setup);
        }

        CStatusMessageList CContextSimulator::getInterpolationMessages(const CCallsign &callsign) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (callsign.isEmpty()) { return CStatusMessageList(); }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CStatusMessageList(); }
            return m_simulatorPlugin.second->getInterpolationMessages(callsign);
        }

        CTime CContextSimulator::getTimeSynchronizationOffset() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CTime(0, CTimeUnit::hrmin()); }
            return m_simulatorPlugin.second->getTimeSynchronizationOffset();
        }

        bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorPluginInfo)
        {
            Q_ASSERT(getIContextApplication());
            Q_ASSERT(getIContextApplication()->isUsingImplementingObject());
            Q_ASSERT(!simulatorPluginInfo.isUnspecified());
            Q_ASSERT(CThreadUtils::isCurrentThreadApplicationThread()); // only run in main thread

            // Is a plugin already loaded?
            if (!m_simulatorPlugin.first.isUnspecified())
            {
                // This can happen, if a listener emitted simulatorStarted twice or two different simulators
                // are running at the same time. In this case, we leave the loaded plugin and just return.
                return false;
            }

            if (!simulatorPluginInfo.isValid())
            {
                CLogMessage(this).error("Illegal plugin");
                return false;
            }

            ISimulatorFactory *factory = m_plugins->getFactory(simulatorPluginInfo.getIdentifier());
            Q_ASSERT_X(factory, Q_FUNC_INFO, "no factory");

            // We assume we run in the same process as the own aircraft context
            // Hence we pass in memory reference to own aircraft object
            Q_ASSERT_X(this->getIContextOwnAircraft()->isUsingImplementingObject(), Q_FUNC_INFO, "Need implementing object");
            Q_ASSERT_X(this->getIContextNetwork()->isUsingImplementingObject(), Q_FUNC_INFO, "Need implementing object");
            IOwnAircraftProvider *ownAircraftProvider = this->getRuntime()->getCContextOwnAircraft();
            IRemoteAircraftProvider *renderedAircraftProvider = this->getRuntime()->getCContextNetwork();
            IClientProvider *clientProvider = this->getRuntime()->getCContextNetwork();
            ISimulator *simulator = factory->create(simulatorPluginInfo, ownAircraftProvider, renderedAircraftProvider, &m_weatherManager, clientProvider);
            Q_ASSERT_X(simulator, Q_FUNC_INFO, "no simulator driver can be created");

            this->setRemoteAircraftProvider(renderedAircraftProvider);

            // use simulator info from ISimulator as it can access the emulated driver settings
            const CSimulatorInfo simInfo = simulator->getSimulatorInfo();
            Q_ASSERT_X(simInfo.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");

            m_modelSetSimulator.set(simInfo);
            const CAircraftModelList modelSetModels = this->getModelSet();
            m_aircraftMatcher.setModelSet(modelSetModels, simInfo, true);
            m_aircraftMatcher.setDefaultModel(simulator->getDefaultModel());

            bool c = connect(simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::onSimulatorStatusChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::physicallyAddingRemoteModelFailed, this, &CContextSimulator::onAddingRemoteAircraftFailed);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::ownAircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::aircraftRenderingChanged, this, &IContextSimulator::aircraftRenderingChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::renderRestrictionsChanged, this, &IContextSimulator::renderRestrictionsChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::interpolationAndRenderingSetupChanged, this, &IContextSimulator::interpolationAndRenderingSetupChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::airspaceSnapshotHandled, this, &IContextSimulator::airspaceSnapshotHandled);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::driverMessages, this, &IContextSimulator::driverMessages);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::requestUiConsoleMessage, this, &IContextSimulator::requestUiConsoleMessage);
            Q_ASSERT(c);

            // log from context to simulator
            c = connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, &CContextSimulator::relayStatusMessageToSimulator);
            Q_ASSERT(c);
            c = connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, this, &CContextSimulator::relayStatusMessageToSimulator);
            Q_ASSERT(c);
            Q_UNUSED(c);

            // Once the simulator signaled it is ready to simulate, add all known aircraft
            m_initallyAddAircraft = true;
            m_wasSimulating = false;
            m_matchingMessages.clear();

            // try to connect to simulator
            const bool connected = simulator->connectTo();
            if (!connected)
            {
                CLogMessage(this).error("Simulator plugin connection to simulator '%1' failed") << simulatorPluginInfo.toQString(true);
                return false;
            }

            simulator->setWeatherActivated(m_isWeatherActivated);

            // when everything is set up connected, update the current plugin info
            m_simulatorPlugin.first = simulatorPluginInfo;
            m_simulatorPlugin.second = simulator;
            m_simulatorPlugin.second->setInterpolationSetupGlobal(m_renderSettings.get());

            // Emit signal after this function completes completely decoupled
            QPointer<CContextSimulator> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                emit this->simulatorPluginChanged(simulatorPluginInfo);
            });

            CLogMessage(this).info("Simulator plugin loaded: '%1' connected: %2")
                    << simulatorPluginInfo.toQString(true)
                    << boolToYesNo(connected);

            return true;
        }

        bool CContextSimulator::listenForSimulator(const CSimulatorPluginInfo &simulatorInfo)
        {
            Q_ASSERT(this->getIContextApplication());
            Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());
            Q_ASSERT(!simulatorInfo.isUnspecified());
            Q_ASSERT(m_simulatorPlugin.first.isUnspecified());

            if (!m_listenersThread.isRunning())
            {
                m_listenersThread.setObjectName("CContextSimulator: Thread for listener " + simulatorInfo.getIdentifier());
                m_listenersThread.start(QThread::LowPriority);
            }

            ISimulatorListener *listener = m_plugins->createListener(simulatorInfo.getIdentifier());
            if (!listener) { return false; }

            if (listener->thread() != &m_listenersThread)
            {
                Q_ASSERT_X(!listener->parent(), Q_FUNC_INFO, "Objects with parent cannot be moved to thread");

                const bool c = connect(listener, &ISimulatorListener::simulatorStarted, this, &CContextSimulator::onSimulatorStarted, Qt::QueuedConnection);
                if (!c)
                {
                    CLogMessage(this).error("Unable to use '%1'") << simulatorInfo.toQString();
                    return false;
                }
                listener->setProperty("isInitialized", true);
                listener->moveToThread(&m_listenersThread);
            }

            // start if not already running
            if (!listener->isRunning())
            {
                const bool s = QMetaObject::invokeMethod(listener, "start", Qt::QueuedConnection);
                Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke method");
                Q_UNUSED(s);
            }
            CLogMessage(this).info("Listening for simulator '%1'") << simulatorInfo.getIdentifier();
            return true;
        }

        void CContextSimulator::listenForAllSimulators()
        {
            const auto plugins = getAvailableSimulatorPlugins();
            for (const CSimulatorPluginInfo &p : plugins)
            {
                Q_ASSERT(!p.isUnspecified());
                if (p.isValid())
                {
                    this->listenForSimulator(p);
                }
            }
        }

        void CContextSimulator::unloadSimulatorPlugin()
        {
            if (!m_simulatorPlugin.first.isUnspecified())
            {
                ISimulator *simulator = m_simulatorPlugin.second;
                if (simulator->isConnected())
                {
                    // we are about to unload an connected simulator
                    this->updateMarkAllAsNotRendered(); // without plugin nothing can be rendered
                    emit this->simulatorStatusChanged(ISimulator::Disconnected);
                }

                m_simulatorPlugin.second = nullptr;
                m_simulatorPlugin.first = CSimulatorPluginInfo();

                Q_ASSERT(this->getIContextNetwork());
                Q_ASSERT(this->getIContextNetwork()->isLocalObject());

                // unload and disconnect
                if (simulator)
                {
                    // disconnect signals and delete
                    simulator->disconnect(this);
                    simulator->unload();
                    simulator->deleteLater();
                    emit this->simulatorPluginChanged(CSimulatorPluginInfo());
                }

                if (m_wasSimulating) { emit this->vitalityLost(); }
                m_wasSimulating = false;
            }
        }

        void CContextSimulator::xCtxAddedRemoteAircraftReadyForModelMatching(const CSimulatedAircraft &remoteAircraft)
        {
            if (!this->isSimulatorAvailable()) { return; }
            const CCallsign callsign = remoteAircraft.getCallsign();
            BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Remote aircraft with empty callsign");
            if (callsign.isEmpty()) { return; }

            // here we find the best simulator model for a resolved model
            // in the first step we already tried to find accurate ICAO codes etc.
            // coming from CAirspaceMonitor::sendReadyForModelMatching
            CStatusMessageList matchingMessages;
            CStatusMessageList *pMatchingMessages = m_enableMatchingMessages ? &matchingMessages : nullptr;
            CAircraftModel aircraftModel = m_aircraftMatcher.getClosestMatch(remoteAircraft, pMatchingMessages);
            Q_ASSERT_X(remoteAircraft.getCallsign() == aircraftModel.getCallsign(), Q_FUNC_INFO, "Mismatching callsigns");
            const CLength cg = m_simulatorPlugin.second->getCGPerModelString(aircraftModel.getModelString());
            if (!cg.isNull()) { aircraftModel.setCG(cg); }
            this->updateAircraftModel(callsign, aircraftModel, this->identifier());

            const CSimulatedAircraft aircraftAfterModelApplied = this->getAircraftInRangeForCallsign(remoteAircraft.getCallsign());
            if (!aircraftAfterModelApplied.hasModelString())
            {
                if (!aircraftAfterModelApplied.hasCallsign()) { return; } // removed
                if (this->isAircraftInRange(aircraftAfterModelApplied.getCallsign())) { return; } // removed, but callsig, we did crosscheck

                // callsign, but no model string
                CLogMessage(this).error("Matching error for '%1', no model string") << aircraftAfterModelApplied.getCallsign().asString();

                CSimulatedAircraft brokenAircraft(aircraftAfterModelApplied);
                brokenAircraft.setEnabled(false);
                brokenAircraft.setRendered(false);
                emit this->aircraftRenderingChanged(brokenAircraft);
                CMatchingUtils::addLogDetailsToList(pMatchingMessages, callsign, QString("Cannot add remote aircraft, no model string: '%1'").arg(brokenAircraft.toQString()));
                return;
            }
            m_simulatorPlugin.second->logicallyAddRemoteAircraft(aircraftAfterModelApplied);
            CMatchingUtils::addLogDetailsToList(pMatchingMessages, callsign, QString("Logically added remote aircraft: %1").arg(aircraftAfterModelApplied.toQString()));

            this->clearMatchingMessages(callsign);
            this->addMatchingMessages(callsign, matchingMessages);

            // done
            emit this->modelMatchingCompleted(aircraftAfterModelApplied);
        }

        void CContextSimulator::xCtxRemovedRemoteAircraft(const CCallsign &callsign)
        {
            if (!this->isSimulatorAvailable()) { return; }
            m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(callsign);
        }

        void CContextSimulator::onSimulatorStatusChanged(ISimulator::SimulatorStatus status)
        {
            m_wasSimulating = m_wasSimulating || status.testFlag(ISimulator::Simulating);
            if (m_initallyAddAircraft && status.testFlag(ISimulator::Simulating))
            {
                // use network to initally add aircraft
                IContextNetwork *networkContext = this->getIContextNetwork();
                Q_ASSERT_X(networkContext, Q_FUNC_INFO, "Need context");
                Q_ASSERT_X(networkContext->isLocalObject(), Q_FUNC_INFO, "Need local object");

                // initially add aircraft
                const CSimulatedAircraftList aircraft = networkContext->getAircraftInRange();
                for (const CSimulatedAircraft &simulatedAircraft : aircraft)
                {
                    BLACK_VERIFY_X(!simulatedAircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "Need callsign");
                    this->xCtxAddedRemoteAircraftReadyForModelMatching(simulatedAircraft);
                }
                m_initallyAddAircraft = false;
            }

            if (!status.testFlag(ISimulator::Connected))
            {
                // we got disconnected, plugin no longer needed
                this->updateMarkAllAsNotRendered(); // without plugin nothing can be rendered
                this->unloadSimulatorPlugin();
                this->restoreSimulatorPlugins();

                if (m_wasSimulating) { emit this->vitalityLost(); }
                m_wasSimulating = false;
            }

            emit this->simulatorStatusChanged(status);
        }

        void CContextSimulator::xCtxTextMessagesReceived(const Network::CTextMessageList &textMessages)
        {
            if (!this->isSimulatorAvailable()) { return; }
            if (!this->getIContextOwnAircraft()) { return; }

            const CSimulatorMessagesSettings settings = m_messageSettings.getThreadLocal();
            const CSimulatedAircraft ownAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
            for (const auto &tm : textMessages)
            {
                if (!settings.relayThisTextMessage(tm, ownAircraft)) { continue; }
                m_simulatorPlugin.second->displayTextMessage(tm);
            }
        }

        void CContextSimulator::xCtxChangedRemoteAircraftModel(const CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator)
        {
            if (CIdentifiable::isMyIdentifier(originator)) { return; }
            if (!this->isSimulatorAvailable()) { return; }
            m_simulatorPlugin.second->changeRemoteAircraftModel(aircraft);
        }

        void CContextSimulator::xCtxChangedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
        {
            if (!this->isSimulatorAvailable()) { return; }
            m_simulatorPlugin.second->changeRemoteAircraftEnabled(aircraft);
        }

        void CContextSimulator::xCtxNetworkConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            BLACK_VERIFY_X(this->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");
            if (to == INetwork::Connected && this->getIContextNetwork())
            {
                m_networkSessionId = this->getIContextNetwork()->getConnectedServer().getServerSessionId();
                if (m_simulatorPlugin.second) // check in case the plugin has been unloaded
                {
                    m_simulatorPlugin.second->setFlightNetworkConnected(true);
                }
            }
            else if (INetwork::isDisconnectedStatus(to))
            {
                m_networkSessionId.clear();
                m_aircraftMatcher.clearMatchingStatistics();
                m_matchingMessages.clear();

                if (m_simulatorPlugin.second) // check in case the plugin has been unloaded
                {
                    m_simulatorPlugin.second->removeAllRemoteAircraft(); // also removes aircraft
                    m_simulatorPlugin.second->setFlightNetworkConnected(false);
                }
            }
        }

        void CContextSimulator::onAddingRemoteAircraftFailed(const CSimulatedAircraft &remoteAircraft, bool disabled, const CStatusMessage &message)
        {
            if (!this->isSimulatorAvailable()) { return; }
            if (disabled) { m_aircraftMatcher.addingRemoteModelFailed(remoteAircraft); }
            emit this->addingRemoteModelFailed(remoteAircraft, disabled, message);
        }

        void CContextSimulator::xCtxUpdateSimulatorCockpitFromContext(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
        {
            if (!this->isSimulatorAvailable()) { return; }
            if (originator.getName().isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

            // update
            m_simulatorPlugin.second->updateOwnSimulatorCockpit(ownAircraft, originator);
        }

        void CContextSimulator::xCtxUpdateSimulatorSelcalFromContext(const CSelcal &selcal, const CIdentifier &originator)
        {
            if (!this->isSimulatorAvailable()) { return; }
            if (originator.getName().isEmpty() || originator == IContextSimulator::InterfaceName()) { return; }

            // update
            m_simulatorPlugin.second->updateOwnSimulatorSelcal(selcal, originator);
        }

        void CContextSimulator::xCtxNetworkRequestedNewAircraft(const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
        {
            if (m_networkSessionId.isEmpty()) { return; }
            m_aircraftMatcher.evaluateStatisticsEntry(m_networkSessionId, callsign, aircraftIcao, airlineIcao, livery);
        }

        void CContextSimulator::relayStatusMessageToSimulator(const BlackMisc::CStatusMessage &message)
        {
            if (!this->isSimulatorAvailable()) { return; }
            const CSimulatorMessagesSettings simMsg = m_messageSettings.getThreadLocal();
            if (simMsg.relayThisStatusMessage(message) && m_simulatorPlugin.second)
            {
                m_simulatorPlugin.second->displayStatusMessage(message);
            }
        }

        void CContextSimulator::changeEnabledSimulators()
        {
            CSimulatorPluginInfo currentPluginInfo = m_simulatorPlugin.first;
            const QStringList enabledSimulators = m_enabledSimulators.getThreadLocal();

            // Unload the current plugin, if it is no longer enabled
            if (!currentPluginInfo.isUnspecified() && !enabledSimulators.contains(currentPluginInfo.getIdentifier()))
            {
                unloadSimulatorPlugin();
                emit simulatorStatusChanged(ISimulator::Disconnected);
            }
            restoreSimulatorPlugins();
        }

        void CContextSimulator::restoreSimulatorPlugins()
        {
            if (!m_simulatorPlugin.first.isUnspecified()) { return; }

            stopSimulatorListeners();
            const QStringList enabledSimulators = m_enabledSimulators.getThreadLocal();
            const CSimulatorPluginInfoList allSimulators = m_plugins->getAvailableSimulatorPlugins();
            for (const CSimulatorPluginInfo &s : allSimulators)
            {
                if (enabledSimulators.contains(s.getIdentifier()))
                {
                    startSimulatorPlugin(s);
                }
            }
        }

        CPixmap CContextSimulator::iconForModel(const QString &modelString) const
        {
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CPixmap(); }

            // load from file
            CStatusMessage msg;
            const CAircraftModel model(this->getModelSet().findFirstByModelStringOrDefault(modelString));
            const CPixmap pm(model.loadIcon(msg));
            if (!msg.isEmpty()) { CLogMessage::preformatted(msg);}
            return pm;
        }

        CStatusMessageList CContextSimulator::getMatchingMessages(const CCallsign &callsign) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_matchingMessages[callsign];
        }

        bool CContextSimulator::isMatchingMessagesEnabled() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_enableMatchingMessages;
        }

        void CContextSimulator::enableMatchingMessages(bool enabled)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enabled; }
            if (m_enableMatchingMessages == enabled) { return; }
            m_enableMatchingMessages = enabled;
            emit CContext::changedLogOrDebugSettings();
        }

        CMatchingStatistics CContextSimulator::getCurrentMatchingStatistics(bool missingOnly) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << missingOnly; }
            const CMatchingStatistics statistics = m_aircraftMatcher.getCurrentStatistics();
            return missingOnly ?
                   statistics.findMissingOnly() :
                   statistics;
        }

        void CContextSimulator::setMatchingSetup(const CAircraftMatcherSetup &setup)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << setup.toQString(); }
            m_aircraftMatcher.setSetup(setup);
            const CStatusMessage msg = m_matchingSettings.setAndSave(setup);
            CLogMessage::preformatted(msg);
        }

        CAircraftMatcherSetup CContextSimulator::getMatchingSetup() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_aircraftMatcher.getSetup();
        }

        CStatusMessageList CContextSimulator::copyFsxTerrainProbe(const CSimulatorInfo &simulator)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << simulator.toQString(); }

            CStatusMessageList msgs;
            if (!simulator.isFsxP3DFamily())
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "Wrong simulator " + simulator.toQString()));
                return msgs;
            }

            const QStringList modelDirs = m_simulatorSettings.getModelDirectoriesOrDefault(simulator);
            if (modelDirs.isEmpty() || modelDirs.front().isEmpty())
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "No model directory"));
                return msgs;
            }

            const int copied = CFsCommonUtil::copyFsxTerrainProbeFiles(modelDirs.front(), msgs);
            if (copied < 1 && !msgs.hasWarningOrErrorMessages())
            {
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "No files copied"));
                return msgs;
            }

            return msgs;
        }

        bool CContextSimulator::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            Q_UNUSED(originator);
            if (commandLine.isEmpty()) { return false; }
            CSimpleCommandParser parser(
            {
                ".plugin", ".drv", ".driver", // forwarded to driver
                ".ris" // rendering interpolator setup
            });
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }
            if (parser.matchesCommand("ris"))
            {
                CInterpolationAndRenderingSetupGlobal rs = this->getInterpolationAndRenderingSetupGlobal();
                const QString p1 = parser.part(1);
                if (p1 == "show")
                {
                    if (this->getIContextApplication())
                    {
                        emit this->getIContextApplication()->requestDisplayOnConsole(rs.toQString(true));
                    }
                    return true;
                }
                if (!parser.hasPart(2)) { return false; }
                const bool on = stringToBool(parser.part(2));
                if (p1 == "debug") { rs.setSimulatorDebuggingMessages(on); }
                else if (p1 == "parts") { rs.setEnabledAircraftParts(on); }
                else { return false; }
                this->setInterpolationAndRenderingSetupGlobal(rs);
                CLogMessage(this, CLogCategory::cmdLine()).info("Setup is: '%1'") << rs.toQString(true);
                return true;
            }
            if (parser.matchesCommand("plugin") || parser.matchesCommand("drv") || parser.matchesCommand("driver"))
            {
                if (!m_simulatorPlugin.second) { return false; }
                return m_simulatorPlugin.second->parseCommandLine(commandLine, originator);
            }
            return false;
        }

        QPointer<ISimulator> CContextSimulator::simulator() const
        {
            if (!this->isSimulatorAvailable() || !m_simulatorPlugin.second) { return nullptr; }
            return m_simulatorPlugin.second;
        }

        void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << aircraftToHighlight << enableHighlight << displayTime; }
            if (!m_simulatorPlugin.second) { return; }
            m_simulatorPlugin.second->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
        }

        bool CContextSimulator::followAircraft(const CCallsign &callsign)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            if (!m_simulatorPlugin.second) { return false; }
            return m_simulatorPlugin.second->followAircraft(callsign);
        }

        void CContextSimulator::recalculateAllAircraft()
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second) { return; }
            return m_simulatorPlugin.second->recalculateAllAircraft();
        }

        bool CContextSimulator::resetToModelMatchingAircraft(const CCallsign &callsign)
        {
            CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
            if (aircraft.getCallsign() != callsign) { return false; } // not found
            this->setAircraftEnabledFlag(callsign, true);  // plain vanilla flag
            this->updateAircraftRendered(callsign, false); // this is flag only anyway
            aircraft.setModel(aircraft.getNetworkModel()); // like originally from network
            this->xCtxAddedRemoteAircraftReadyForModelMatching(aircraft);
            return true;
        }

        void CContextSimulator::setWeatherActivated(bool activated)
        {
            m_isWeatherActivated = activated;

            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return; }
            m_simulatorPlugin.second->setWeatherActivated(activated);
        }

        void CContextSimulator::requestWeatherGrid(const Weather::CWeatherGrid &weatherGrid, const CIdentifier &identifier)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << identifier; }
            m_weatherManager.requestWeatherGrid(weatherGrid, identifier);
        }

        int CContextSimulator::doMatchingsAgain()
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();
            if (callsigns.isEmpty()) { return 0; }
            int delayMs = 25;
            QPointer<CContextSimulator> myself(this);
            for (const CCallsign &cs : callsigns)
            {
                QTimer::singleShot(delayMs, this, [ = ]
                {
                    if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                    this->doMatchingAgain(cs);
                });
                delayMs += 25;
            }
            return callsigns.size();
        }

        bool CContextSimulator::doMatchingAgain(const CCallsign &callsign)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign.asString(); }
            if (!this->isAircraftInRange(callsign)) { return false; }
            if (!this->isSimulatorAvailable()) { return false; }

            QPointer<CContextSimulator> myself(this);
            QTimer::singleShot(2500, this, [ = ]
            {
                if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                const CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
                if (!aircraft.hasCallsign()) { return; } // no longer valid
                CSimulatedAircraft resetAircraft(aircraft);
                resetAircraft.resetToNetworkModel();
                this->xCtxAddedRemoteAircraftReadyForModelMatching(resetAircraft);
            });
            return false;
        }

        void CContextSimulator::onSimulatorStarted(const CSimulatorPluginInfo &info)
        {
            this->stopSimulatorListeners();
            this->loadSimulatorPlugin(info);

            // if we have enabled messages, we will disable if size getting too high
            if (m_enableMatchingMessages)
            {
                const QPointer<CContextSimulator> myself(this);
                QTimer::singleShot(5000, this, [ = ]
                {
                    if (!myself) { return; }
                    if (m_aircraftMatcher.getModelSetCount() > MatchingLogMaxModelSetSize)
                    {
                        this->enableMatchingMessages(false);
                    }
                });
            }
        }

        void CContextSimulator::stopSimulatorListeners()
        {
            for (const CSimulatorPluginInfo &info : getAvailableSimulatorPlugins())
            {
                ISimulatorListener *listener = m_plugins->getListener(info.getIdentifier());
                if (listener)
                {
                    const bool s = QMetaObject::invokeMethod(listener, "stop");
                    Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot invoke stop");
                    Q_UNUSED(s);
                }
            }
        }

        void CContextSimulator::addMatchingMessages(const CCallsign &callsign, const CStatusMessageList &messages)
        {
            if (callsign.isEmpty()) { return; }
            if (messages.isEmpty()) { return; }
            if (!m_enableMatchingMessages) { return; }
            if (m_matchingMessages.contains(callsign))
            {
                CStatusMessageList &msgs = m_matchingMessages[callsign];
                msgs.push_back(messages);
            }
            else
            {
                m_matchingMessages.insert(callsign, messages);
            }
        }

        void CContextSimulator::clearMatchingMessages(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return; }
            m_matchingMessages.remove(callsign);
        }

        void CContextSimulator::initByLastUsedModelSet()
        {
            // no models in matcher, but in cache, we can set them as default
            const CSimulatorInfo simulator(m_modelSetSimulator.get());
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
            const CAircraftModelList models(this->getModelSet());
            CLogMessage(this).info("Init aircraft matcher with %1 models from set for '%2'") << models.size() << simulator.toQString();
            m_aircraftMatcher.setModelSet(models, simulator, false);
        }
    } // namespace
} // namespace
