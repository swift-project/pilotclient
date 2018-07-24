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
            // m_enableMatchingMessages = CBuildConfig::isLocalDeveloperDebugBuild() ||  sApp->isDeveloperFlagSet();
            m_enableMatchingMessages = true; // there seems to be no big disadavantage in always enabling it
            m_plugins->collectPlugins();
            this->restoreSimulatorPlugins();

            connect(&m_weatherManager, &CWeatherManager::weatherGridReceived, this, &CContextSimulator::weatherGridReceived);
            connect(&CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance(), &CCentralMultiSimulatorModelSetCachesProvider::cacheChanged, this, &CContextSimulator::modelSetChanged);

            // deferred init of last model set, if no other data are set in meantime
            const QPointer<CContextSimulator> myself(this);
            QTimer::singleShot(1250, this, [ = ]
            {
                if (!myself) { return; }
                this->initByLastUsedModelSet();
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
            QMetaObject::invokeMethod(listener, "stop");
        }

        int CContextSimulator::getSimulatorStatus() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return 0; }

            Q_ASSERT_X(m_simulatorPlugin.second, Q_FUNC_INFO, "Missing simulator");
            return m_simulatorPlugin.second->getSimulatorStatus();
        }

        CSimulatorPluginInfo CContextSimulator::getSimulatorPluginInfo() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return CSimulatorPluginInfo(); }

            Q_ASSERT(m_simulatorPlugin.second);
            if (m_simulatorPlugin.first.getSimulator().contains("emulated", Qt::CaseInsensitive)) { return m_simulatorPlugin.second->getSimulatorPluginInfo(); }
            return m_simulatorPlugin.first;
        }

        CSimulatorInternals CContextSimulator::getSimulatorInternals() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified())
            {
                return CSimulatorInternals();
            }

            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->getSimulatorInternals();
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

        CAircraftModelList CContextSimulator::getModelSet() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const CSimulatorInfo simulator = m_modelSetSimulator.get();
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
            m_aircraftMatcher.setModelSet(models, simulator);
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
            const CSimulatorInfo sims = this->simulatorsWithInitializedModelSet();
            if (sims.isNoSimulator())
            {
                msgs.push_back(CStatusMessage(this).error("No model set so far, you need at least one model set. Hint: You can create a model set in the mapping tool, or copy an existing set in the launcher."));
            }
            else if (sims.isXPlane() || CSimulatorInfo(m_enabledSimulators.get()).isXPlane())
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

        QStringList CContextSimulator::getModelSetCompleterStrings(bool sorted) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << sorted; }
            return this->getModelSet().toCompleterStrings(sorted);
        }

        int CContextSimulator::getModelSetCount() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return 0; }

            Q_ASSERT(m_simulatorPlugin.second);
            return this->getModelSet().size();
        }

        CAircraftModelList CContextSimulator::getModelSetModelsStartingWith(const QString &modelString) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << modelString; }
            if (m_simulatorPlugin.first.isUnspecified())
            {
                return CAircraftModelList();
            }

            Q_ASSERT(m_simulatorPlugin.second);
            return this->getModelSet().findModelsStartingWith(modelString);
        }

        bool CContextSimulator::setTimeSynchronization(bool enable, const CTime &offset)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return false; }

            Q_ASSERT(m_simulatorPlugin.second);
            const bool c = m_simulatorPlugin.second->setTimeSynchronization(enable, offset);
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

        CInterpolationAndRenderingSetupGlobal CContextSimulator::getInterpolationAndRenderingSetupGlobal() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return CInterpolationAndRenderingSetupGlobal(); }
            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->getInterpolationSetupGlobal();
        }

        CInterpolationSetupList CContextSimulator::getInterpolationAndRenderingSetupsPerCallsign() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return CInterpolationSetupList(); }
            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->getInterpolationSetupsPerCallsign();
        }

        CInterpolationAndRenderingSetupPerCallsign CContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault(const CCallsign &callsign) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return CInterpolationAndRenderingSetupPerCallsign(); }
            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->getInterpolationSetupPerCallsignOrDefault(callsign);
        }

        bool CContextSimulator::setInterpolationAndRenderingSetupsPerCallsign(const CInterpolationSetupList &setups, bool ignoreSameAsGlobal)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return false; }
            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->setInterpolationSetupsPerCallsign(setups, ignoreSameAsGlobal);
        }

        void CContextSimulator::setInterpolationAndRenderingSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << setup; }
            if (m_simulatorPlugin.first.isUnspecified()) { return; }
            Q_ASSERT(m_simulatorPlugin.second);
            m_simulatorPlugin.second->setInterpolationSetupGlobal(setup);
        }

        CTime CContextSimulator::getTimeSynchronizationOffset() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return CTime(0, CTimeUnit::hrmin()); }
            Q_ASSERT(m_simulatorPlugin.second);
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
            m_aircraftMatcher.setModelSet(modelSetModels, simInfo);
            m_aircraftMatcher.setDefaultModel(simulator->getDefaultModel());

            bool c = connect(simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::onSimulatorStatusChanged);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::physicallyAddingRemoteModelFailed, this, &CContextSimulator::addingRemoteAircraftFailed);
            Q_ASSERT(c);
            c = connect(simulator, &ISimulator::receivedRequestedElevation, this, &CContextSimulator::onReceivedRequestedElevation);
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
            m_initallyAddAircrafts = true;
            m_matchingMessages.clear();

            // try to connect to simulator
            const bool connected = simulator->connectTo();
            simulator->setWeatherActivated(m_isWeatherActivated);

            // when everything is set up connected, update the current plugin info
            m_simulatorPlugin.first = simulatorPluginInfo;
            m_simulatorPlugin.second = simulator;

            // Emit signal after this function completes
            QTimer::singleShot(0, this, [ = ]
            {
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

                const bool c = connect(listener, &ISimulatorListener::simulatorStarted, this, &CContextSimulator::onSimulatorStarted);
                if (!c)
                {
                    CLogMessage(this).error("Unable to use '%1'") << simulatorInfo.toQString();
                    return false;
                }
                listener->setProperty("isInitialized", true);
                listener->moveToThread(&m_listenersThread);
            }

            const bool s = QMetaObject::invokeMethod(listener, "start", Qt::QueuedConnection);
            Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke method");
            Q_UNUSED(s);

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
                    sim->disconnect(this);
                    sim->unload();
                    sim->deleteLater();
                    emit this->simulatorPluginChanged(CSimulatorPluginInfo());
                }
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
                CMatchingUtils::addLogDetailsToList(pMatchingMessages, callsign, QString("Cannot add remote aircraft, no model string: %1").arg(brokenAircraft.toQString()));
                return;
            }
            m_simulatorPlugin.second->logicallyAddRemoteAircraft(aircraftAfterModelApplied);
            CMatchingUtils::addLogDetailsToList(pMatchingMessages, callsign, QString("Logically added remote aircraft: %1").arg(aircraftAfterModelApplied.toQString()));
            this->addMatchingMessages(callsign, matchingMessages);
            emit this->modelMatchingCompleted(aircraftAfterModelApplied);
        }

        void CContextSimulator::xCtxRemovedRemoteAircraft(const CCallsign &callsign)
        {
            if (!this->isSimulatorAvailable()) { return; }
            m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(callsign);
        }

        void CContextSimulator::onSimulatorStatusChanged(ISimulator::SimulatorStatus status)
        {
            if (m_initallyAddAircrafts && status.testFlag(ISimulator::Simulating))
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
                m_initallyAddAircrafts = false;
            }
            if (!status.testFlag(ISimulator::Connected))
            {
                // we got disconnected, plugin no longer needed
                unloadSimulatorPlugin();
                restoreSimulatorPlugins();
            }
            emit simulatorStatusChanged(status);
        }

        void CContextSimulator::onModelSetChanged(const CSimulatorInfo &simulator)
        {
            Q_UNUSED(simulator);
            emit this->modelSetChanged(simulator);
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

        void CContextSimulator::onCockpitChangedFromSimulator(const CSimulatedAircraft &ownAircraft)
        {
            Q_ASSERT(getIContextOwnAircraft());
            emit getIContextOwnAircraft()->changedAircraftCockpit(ownAircraft, IContextSimulator::InterfaceName());
        }

        void CContextSimulator::onReceivedRequestedElevation(const CElevationPlane &plane, const CCallsign &callsign)
        {
            if (!this->isSimulatorAvailable()) { return; }
            emit this->receivedRequestedElevation(plane, callsign);
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
            BLACK_VERIFY_X(getIContextNetwork(), Q_FUNC_INFO, "Missing network context");
            if (to == INetwork::Connected && this->getIContextNetwork())
            {
                m_networkSessionId = this->getIContextNetwork()->getConnectedServer().getServerSessionId();
            }
            else if (INetwork::isDisconnectedStatus(to))
            {
                m_networkSessionId.clear();
                m_aircraftMatcher.clearMatchingStatistics();
                m_matchingMessages.clear();

                // check in case the plugin has been unloaded
                if (m_simulatorPlugin.second)
                {
                    const CStatusMessageList verifyMessages = m_simulatorPlugin.second->debugVerifyStateAfterAllAircraftRemoved();
                    m_simulatorPlugin.second->clearAllRemoteAircraftData();
                    if (!verifyMessages.isEmpty()) { emit this->driverMessages(verifyMessages); }
                }
            }
        }

        void CContextSimulator::addingRemoteAircraftFailed(const CSimulatedAircraft &remoteAircraft, const CStatusMessage &message)
        {
            if (!this->isSimulatorAvailable()) { return; }
            emit this->addingRemoteModelFailed(remoteAircraft, message);
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
            if (simMsg.relayThisStatusMessage(message))
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
            if (m_simulatorPlugin.first.isUnspecified()) { return CPixmap(); }
            Q_ASSERT_X(m_simulatorPlugin.second, Q_FUNC_INFO, "Missing simulator");

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

        ISimulator *CContextSimulator::simulator() const
        {
            if (!this->isSimulatorAvailable()) { return nullptr; }
            return m_simulatorPlugin.second;
        }

        void CContextSimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << aircraftToHighlight << enableHighlight << displayTime; }
            Q_ASSERT(m_simulatorPlugin.second);
            m_simulatorPlugin.second->highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
        }

        bool CContextSimulator::followAircraft(const CCallsign &callsign)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            Q_ASSERT(m_simulatorPlugin.second);
            return m_simulatorPlugin.second->followAircraft(callsign);
        }

        bool CContextSimulator::resetToModelMatchingAircraft(const CCallsign &callsign)
        {
            CSimulatedAircraft aircraft = getAircraftInRangeForCallsign(callsign);
            if (aircraft.getCallsign() != callsign) { return false; } // not found
            aircraft.setModel(aircraft.getNetworkModel());
            xCtxAddedRemoteAircraftReadyForModelMatching(aircraft);
            return true;
        }

        void CContextSimulator::setWeatherActivated(bool activated)
        {
            m_isWeatherActivated = activated;

            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_simulatorPlugin.first.isUnspecified()) { return; }
            m_simulatorPlugin.second->setWeatherActivated(activated);
        }

        void CContextSimulator::requestWeatherGrid(const Weather::CWeatherGrid &weatherGrid, const CIdentifier &identifier)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << identifier; }
            m_weatherManager.requestWeatherGrid(weatherGrid, identifier);
        }

        void CContextSimulator::onSimulatorStarted(const CSimulatorPluginInfo &info)
        {
            stopSimulatorListeners();
            loadSimulatorPlugin(info);
        }

        void CContextSimulator::stopSimulatorListeners()
        {
            for (const auto &info : getAvailableSimulatorPlugins())
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

        void CContextSimulator::initByLastUsedModelSet()
        {
            // no models in matcher, but in cache, we can set them as default
            const CSimulatorInfo simulator(m_modelSetSimulator.get());
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
            const CAircraftModelList models(this->getModelSet());
            CLogMessage(this).info("Init aircraft matcher with %1 models from set for '%2'") << models.size() << simulator.toQString();
            m_aircraftMatcher.setModelSet(models, simulator);
        }
    } // namespace
} // namespace
