// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextsimulatorimpl.h"

#include <QMetaObject>
#include <QPointer>
#include <QStringList>
#include <QThread>
#include <Qt>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "core/application.h"
#include "core/context/contextapplication.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextnetworkimpl.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextownaircraftimpl.h"
#include "core/corefacade.h"
#include "core/db/databaseutils.h"
#include "core/pluginmanagersimulator.h"
#include "core/simulator.h"
#include "misc/aviation/callsign.h"
#include "misc/dbusserver.h"
#include "misc/logcategories.h"
#include "misc/loghandler.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/pq/units.h"
#include "misc/simplecommandparser.h"
#include "misc/simulation/matchingutils.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/xplane/xplaneutil.h"
#include "misc/statusmessage.h"
#include "misc/threadutils.h"
#include "misc/verify.h"

using namespace swift::config;
using namespace swift::core::db;
using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::xplane;
using namespace swift::misc::geo;
using namespace swift::misc::weather;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::simulation::data;
using namespace std::chrono_literals;

namespace swift::core::context
{
    CContextSimulator::CContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
        : IContextSimulator(mode, runtime), CIdentifiable(this), m_plugins(new CPluginManagerSimulator(this))
    {
        this->setObjectName("CContextSimulator");
        CContextSimulator::registerHelp();

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");
        MatchingLog logMatchingMessages =
            CBuildConfig::isLocalDeveloperDebugBuild() ? MatchingLogAll : MatchingLogSimplified;
        m_logMatchingMessages = logMatchingMessages;
        m_plugins->collectPlugins();
        this->restoreSimulatorPlugins();

        connect(&m_aircraftMatcher, &CAircraftMatcher::setupChanged, this, &CContextSimulator::matchingSetupChanged);
        connect(&CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance(),
                &CCentralMultiSimulatorModelSetCachesProvider::cacheChanged, this, &CContextSimulator::modelSetChanged);

        // deferred init of last model set, if no other data are set in meantime
        const QPointer<CContextSimulator> myself(this);
        QTimer::singleShot(2500ms, this, [=] {
            if (!myself) { return; }
            this->initByLastUsedModelSet();
            m_aircraftMatcher.setSetup(m_matchingSettings.get());
            if (m_aircraftMatcher.getModelSetCount() <= MatchingLogMaxModelSetSize)
            {
                this->enableMatchingMessages(logMatchingMessages);
            }
        });

        // Validation
        m_validator = new CBackgroundValidation(this);
        this->setValidator(this->getSimulatorPluginInfo().getSimulator());
        connect(this, &CContextSimulator::simulatorChanged, this, &CContextSimulator::setValidator);
        connect(m_validator, &CBackgroundValidation::validated, this, &CContextSimulator::validatedModelSet,
                Qt::QueuedConnection);

        m_validator->start(QThread::LowestPriority);
        m_validator->startUpdating(60);
    }

    // For validation we need simulator directory and model directory
    // this function is called at start (simulator=0) and when there is an active connection to a simulator
    void CContextSimulator::setValidator(const CSimulatorInfo &simulator)
    {
        if (simulator.isSingleSimulator())
        {
            const QString simDir = m_multiSimulatorSettings.getSimulatorDirectoryOrDefault(simulator);
            const QStringList modelDirList = m_multiSimulatorSettings.getModelDirectoriesOrDefault(simulator);
            m_validator->setCurrentSimulator(simulator, simDir, modelDirList);
        }
        else { m_validator->setCurrentSimulator(CSimulatorInfo::None, {}, {}); }
    }

    void CContextSimulator::registerWithDBus(CDBusServer *server)
    {
        if (!server || getMode() != CCoreFacadeConfig::LocalInDBusServer) { return; }
        server->addObject(CContextSimulator::ObjectPath(), this);
    }

    bool CContextSimulator::isSimulatorPluginAvailable() const
    {
        return m_simulatorPlugin.second && IContextSimulator::isSimulatorAvailable();
    }

    CContextSimulator::~CContextSimulator() { this->gracefulShutdown(); }

    void CContextSimulator::gracefulShutdown()
    {
        if (m_validator)
        {
            disconnect(m_validator);
            m_validator->quitAndWait();
            m_validator->deleteLater();
            m_validator = nullptr;
        }
        this->stopSimulatorListeners();
        this->disconnect();
        this->unloadSimulatorPlugin();

        // give listeners a head start
        // if simconnect is running remotely it can take a while until it shutdowns
        m_listenersThread.quit();
        m_listenersThread.wait(10 * 1000);
    }

    CSimulatorPluginInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        if (!m_plugins) { return {}; }
        return m_plugins->getAvailableSimulatorPlugins();
    }

    CSimulatorSettings CContextSimulator::getSimulatorSettings() const
    {
        const CSimulatorPluginInfo p = this->getSimulatorPluginInfo();
        if (!p.isValid()) { return {}; }
        const CSimulatorInfo sim = p.getSimulatorInfo();
        if (!sim.isSingleSimulator()) { return {}; }
        return m_multiSimulatorSettings.getSettings(sim);
    }

    bool CContextSimulator::setSimulatorSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator)
    {
        if (!simulator.isSingleSimulator()) { return false; }
        const CSimulatorSettings simSettings = m_multiSimulatorSettings.getSettings(simulator);
        if (simSettings == settings) { return false; }
        const CStatusMessage msg = m_multiSimulatorSettings.setSettings(settings, simulator);
        CLogMessage::preformatted(msg);
        emit this->simulatorSettingsChanged();
        return true;
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
        QMetaObject::invokeMethod(listener, &ISimulatorListener::stop, Qt::QueuedConnection);
    }

    int CContextSimulator::checkListeners()
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_plugins) { return 0; }
        return m_plugins->checkAvailableListeners();
    }

    ISimulator::SimulatorStatus CContextSimulator::getSimulatorStatus() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return ISimulator::Unspecified; }
        return m_simulatorPlugin.second->getSimulatorStatus();
    }

    CSimulatorPluginInfo CContextSimulator::getSimulatorPluginInfo() const
    {
        static const CSimulatorPluginInfo unspecified;
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return unspecified; }
        if (m_simulatorPlugin.first.getSimulator().contains("emulated", Qt::CaseInsensitive))
        {
            return m_simulatorPlugin.second->getSimulatorPluginInfo();
        }
        return m_simulatorPlugin.first;
    }

    CSimulatorInternals CContextSimulator::getSimulatorInternals() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CSimulatorInternals(); }
        return m_simulatorPlugin.second->getSimulatorInternals();
    }

    CAirportList CContextSimulator::getAirportsInRange(bool recalculateDistance) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        // If no ISimulator object is available, return a dummy.
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CAirportList(); }
        return m_simulatorPlugin.second->getAirportsInRange(recalculateDistance);
    }

    CAircraftModelList CContextSimulator::getModelSet() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        const CSimulatorInfo simulator = this->getModelSetLoaderSimulator();
        if (!simulator.isSingleSimulator()) { return CAircraftModelList(); }

        CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
    }

    CSimulatorInfo CContextSimulator::getModelSetLoaderSimulator() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (m_simulatorPlugin.second)
        {
            if (m_simulatorPlugin.second->isConnected())
            {
                if (m_simulatorPlugin.second->isEmulatedDriver())
                {
                    // specialized version returning the "emulated info"
                    return this->getSimulatorPluginInfo().getSimulatorInfo();
                }
                return m_simulatorPlugin.first.getSimulatorInfo();
            }
        }
        return m_modelSetSimulator.get();
    }

    void CContextSimulator::setModelSetLoaderSimulator(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (this->isSimulatorAvailable()) { return; } // if a plugin is loaded, do ignore this
        m_modelSetSimulator.set(simulator);
        const CAircraftModelList models = this->getModelSet(); // cache synced
        m_aircraftMatcher.setModelSet(models, simulator, false);
    }

    CSimulatorInfo CContextSimulator::simulatorsWithInitializedModelSet() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().simulatorsWithInitializedCache();
    }

    CStatusMessageList CContextSimulator::verifyPrerequisites() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        CStatusMessageList msgs;
        const CSimulatorInfo simulators = this->simulatorsWithInitializedModelSet();
        if (simulators.isNoSimulator())
        {
            msgs.push_back(CStatusMessage(this).validationError(
                u"No model set so far, you need at least one model set. Hint: You can create a model set in the "
                u"mapping tool, or copy an existing set in the launcher."));
        }
        return msgs;
    }

    QStringList CContextSimulator::getModelSetStrings() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->getModelSet().getModelStringList(false);
    }

    bool CContextSimulator::isKnownModelInSet(const QString &modelString) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        const bool known = this->getModelSet().containsModelString(modelString);
        return known;
    }

    int CContextSimulator::removeModelsFromSet(const CAircraftModelList &removeModels)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (removeModels.isEmpty()) { return 0; }
        const CSimulatorInfo simulator = m_modelSetSimulator.get();
        if (!simulator.isSingleSimulator()) { return 0; }

        CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        CAircraftModelList models =
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
        const int removed = models.removeModelsWithString(removeModels, Qt::CaseInsensitive);
        if (removed > 0)
        {
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().setCachedModels(models, simulator);
        }
        return removed;
    }

    QStringList CContextSimulator::getModelSetCompleterStrings(bool sorted) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << sorted; }
        return this->getModelSet().toCompleterStrings(sorted);
    }

    int CContextSimulator::getModelSetCount() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return 0; }
        return this->getModelSet().size();
    }

    void CContextSimulator::disableModelsForMatching(const CAircraftModelList &removedModels, bool incremental)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return; }
        m_aircraftMatcher.disableModelsForMatching(removedModels, incremental);
    }

    CAircraftModelList CContextSimulator::getDisabledModelsForMatching() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CAircraftModelList(); }
        return m_aircraftMatcher.getDisabledModelsForMatching();
    }

    void CContextSimulator::restoreDisabledModels()
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return; }
        m_aircraftMatcher.restoreDisabledModels();
    }

    bool CContextSimulator::isValidationInProgress() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_validator) { return false; }
        return m_validator->isValidating();
    }

    bool CContextSimulator::triggerModelSetValidation(const CSimulatorInfo &simulator)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_validator) { return false; }
        const QString simDir =
            simulator.isSingleSimulator() ? m_multiSimulatorSettings.getSimulatorDirectoryOrDefault(simulator) : "";
        return m_validator->triggerValidation(simulator, simDir);
    }

    CAircraftModelList CContextSimulator::getModelSetModelsStartingWith(const QString &modelString) const
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << modelString;
        }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CAircraftModelList(); }

        return this->getModelSet().findModelsStartingWith(modelString);
    }

    CInterpolationAndRenderingSetupGlobal CContextSimulator::getInterpolationAndRenderingSetupGlobal() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return m_renderSettings.get(); }
        return m_simulatorPlugin.second->getInterpolationSetupGlobal();
    }

    CInterpolationSetupList CContextSimulator::getInterpolationAndRenderingSetupsPerCallsign() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CInterpolationSetupList(); }
        return m_simulatorPlugin.second->getInterpolationSetupsPerCallsign();
    }

    CInterpolationAndRenderingSetupPerCallsign
    CContextSimulator::getInterpolationAndRenderingSetupPerCallsignOrDefault(const CCallsign &callsign) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified())
        {
            return CInterpolationAndRenderingSetupPerCallsign();
        }
        return m_simulatorPlugin.second->getInterpolationSetupPerCallsignOrDefault(callsign);
    }

    bool CContextSimulator::setInterpolationAndRenderingSetupsPerCallsign(const CInterpolationSetupList &setups,
                                                                          bool ignoreSameAsGlobal)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return false; }
        return m_simulatorPlugin.second->setInterpolationSetupsPerCallsign(setups, ignoreSameAsGlobal);
    }

    void CContextSimulator::setInterpolationAndRenderingSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << setup; }

        // anyway save for future reference
        const CStatusMessage m = m_renderSettings.setAndSave(setup);
        CLogMessage::preformatted(m);

        // transfer to sim
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return; }
        m_simulatorPlugin.second->setInterpolationSetupGlobal(setup);
    }

    CStatusMessageList CContextSimulator::getInterpolationMessages(const CCallsign &callsign) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (callsign.isEmpty()) { return CStatusMessageList(); }
        if (!m_simulatorPlugin.second || m_simulatorPlugin.first.isUnspecified()) { return CStatusMessageList(); }
        return m_simulatorPlugin.second->getInterpolationMessages(callsign);
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorPluginInfo &simulatorPluginInfo)
    {
        Q_ASSERT(this->getIContextApplication());
        Q_ASSERT(this->getIContextApplication()->isUsingImplementingObject());
        Q_ASSERT(!simulatorPluginInfo.isUnspecified());
        Q_ASSERT(CThreadUtils::thisIsMainThread()); // only run in main thread

        // Is a plugin already loaded?
        if (!m_simulatorPlugin.first.isUnspecified())
        {
            // This can happen, if a listener emitted simulatorStarted twice or two different simulators
            // are running at the same time. In this case, we leave the loaded plugin and just return.
            return false;
        }

        if (!simulatorPluginInfo.isValid())
        {
            CLogMessage(this).error(u"Illegal plugin");
            return false;
        }

        ISimulatorFactory *factory = m_plugins->getFactory(simulatorPluginInfo.getIdentifier());
        Q_ASSERT_X(factory, Q_FUNC_INFO, "no factory");

        // We assume we run in the same process as the own aircraft context
        // Hence we pass in memory reference to own aircraft object
        Q_ASSERT_X(this->getIContextOwnAircraft()->isUsingImplementingObject(), Q_FUNC_INFO,
                   "Need implementing object");
        Q_ASSERT_X(this->getIContextNetwork()->isUsingImplementingObject(), Q_FUNC_INFO, "Need implementing object");
        IOwnAircraftProvider *ownAircraftProvider = this->getRuntime()->getCContextOwnAircraft();
        IRemoteAircraftProvider *renderedAircraftProvider = this->getRuntime()->getCContextNetwork();
        IClientProvider *clientProvider = this->getRuntime()->getCContextNetwork();
        ISimulator *simulator =
            factory->create(simulatorPluginInfo, ownAircraftProvider, renderedAircraftProvider, clientProvider);
        Q_ASSERT_X(simulator, Q_FUNC_INFO, "no simulator driver can be created");

        this->setRemoteAircraftProvider(renderedAircraftProvider);

        // use simulator info from ISimulator as it can access the emulated driver settings
        CSimulatorInfo simInfo = simulator->getSimulatorInfo();
        if (!simInfo.isSingleSimulator())
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Invalid simulator from plugin");
            simInfo = CSimulatorInfo::p3d();
            if (simulator->isEmulatedDriver())
            {
                CLogMessage(this).error(
                    u"Emulated driver does NOT provide valid simulator, using default '%1', plugin: '%2'")
                    << simInfo.toQString(true) << simulatorPluginInfo.toQString(true);
            }
            else
            {
                CLogMessage(this).error(u"Invalid driver using default '%1', plugin: '%2'")
                    << simInfo.toQString(true) << simulatorPluginInfo.toQString(true);
            }
        }
        Q_ASSERT_X(simInfo.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");

        m_modelSetSimulator.set(simInfo);
        const CAircraftModelList modelSetModels = this->getModelSet(); // synced
        m_aircraftMatcher.setModelSet(modelSetModels, simInfo, true);
        m_aircraftMatcher.setDefaultModel(simulator->getDefaultModel());

        bool c =
            connect(simulator, &ISimulator::simulatorStatusChanged, this, &CContextSimulator::onSimulatorStatusChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::physicallyAddingRemoteModelFailed, this,
                    &CContextSimulator::onAddingRemoteAircraftFailed);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::ownAircraftModelChanged, this,
                    &CContextSimulator::onOwnSimulatorModelChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::aircraftRenderingChanged, this,
                    &IContextSimulator::aircraftRenderingChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::renderRestrictionsChanged, this,
                    &IContextSimulator::renderRestrictionsChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::interpolationAndRenderingSetupChanged, this,
                    &IContextSimulator::interpolationAndRenderingSetupChanged);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::airspaceSnapshotHandled, this, &IContextSimulator::airspaceSnapshotHandled);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::driverMessages, this, &IContextSimulator::driverMessages);
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::autoPublishDataWritten, this, &IContextSimulator::autoPublishDataWritten);
        Q_ASSERT(c);

        // disconnect for X-Plane FPS below 20
        c = connect(simulator, &ISimulator::insufficientFrameRateDetected, this, [this](bool fatal) {
            if (fatal) { emit this->vitalityLost(); }
        });
        Q_ASSERT(c);
        c = connect(simulator, &ISimulator::insufficientFrameRateDetected, this,
                    &IContextSimulator::insufficientFrameRateDetected);
        Q_ASSERT(c);

        // log from context to simulator
        c = connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this,
                    &CContextSimulator::relayStatusMessageToSimulator);
        Q_ASSERT(c);
        c = connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, this,
                    &CContextSimulator::relayStatusMessageToSimulator);
        Q_ASSERT(c);
        Q_UNUSED(c)

        // Once the simulator signaled it is ready to simulate, add all known aircraft
        m_initallyAddAircraft = true;
        m_wasSimulating = false;
        m_matchingMessages.clear();
        m_failoverAddingCounts.clear();

        // try to connect to simulator
        const bool connected = simulator->connectTo();
        if (!connected)
        {
            CLogMessage(this).error(u"Simulator plugin connection to simulator '%1' failed")
                << simulatorPluginInfo.toQString(true);
            return false;
        }

        // when everything is set up connected, update the current plugin info
        m_simulatorPlugin.first = simulatorPluginInfo;
        m_simulatorPlugin.second = simulator;
        m_simulatorPlugin.second->setInterpolationSetupGlobal(m_renderSettings.get());

        // Emit signal after this function completes completely decoupled
        QPointer<CContextSimulator> myself(this);
        QTimer::singleShot(25, this, [=] {
            if (!myself || !sApp || sApp->isShuttingDown()) { return; }
            if (m_simulatorPlugin.second)
            {
                CLogMessage(this).info(u"Simulator plugin loaded: '%1' connected: %2")
                    << simulatorPluginInfo.toQString(true) << boolToYesNo(connected);

                // use the real driver as this will also work eith emulated driver
                emit this->simulatorPluginChanged(m_simulatorPlugin.second->getSimulatorPluginInfo());
                emit this->simulatorChanged(m_simulatorPlugin.second->getSimulatorInfo());
            }
        });

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

            const bool c = connect(listener, &ISimulatorListener::simulatorStarted, this,
                                   &CContextSimulator::onSimulatorStarted, Qt::QueuedConnection);
            if (!c)
            {
                CLogMessage(this).error(u"Unable to use '%1'") << simulatorInfo.toQString();
                return false;
            }
            listener->setProperty("isInitialized", true);
            listener->moveToThread(&m_listenersThread);
        }

        // start if not already running
        if (!listener->isRunning())
        {
            const bool s = QMetaObject::invokeMethod(listener, &ISimulatorListener::start, Qt::QueuedConnection);
            Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke method");
            Q_UNUSED(s)
        }
        CLogMessage(this).info(u"Listening for simulator '%1'") << simulatorInfo.getIdentifier();
        return true;
    }

    void CContextSimulator::listenForAllSimulators()
    {
        const auto plugins = getAvailableSimulatorPlugins();
        for (const CSimulatorPluginInfo &p : plugins)
        {
            Q_ASSERT(!p.isUnspecified());
            if (p.isValid()) { this->listenForSimulator(p); }
        }
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (!m_simulatorPlugin.first.isUnspecified())
        {
            ISimulator *simulator = m_simulatorPlugin.second;
            if (simulator && simulator->isConnected())
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
                emit this->simulatorChanged(CSimulatorInfo());
            }

            if (m_wasSimulating) { emit this->vitalityLost(); }
            m_wasSimulating = false;
        }
    }

    void CContextSimulator::xCtxAddedRemoteAircraftReadyForModelMatching(const CSimulatedAircraft &remoteAircraft)
    {
        if (!this->isSimulatorPluginAvailable()) { return; }

        const CCallsign callsign = remoteAircraft.getCallsign();
        SWIFT_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Remote aircraft with empty callsign");
        if (callsign.isEmpty()) { return; }

        // here we find the best simulator model for a resolved model
        // in the first step we already tried to find accurate ICAO codes etc.
        // coming from CAirspaceMonitor::sendReadyForModelMatching
        MatchingLog whatToLog = m_logMatchingMessages;
        CStatusMessageList matchingMessages;
        CStatusMessageList *pMatchingMessages = m_logMatchingMessages > 0 ? &matchingMessages : nullptr;
        CAircraftModel aircraftModel =
            m_aircraftMatcher.getClosestMatch(remoteAircraft, whatToLog, pMatchingMessages, true);
        Q_ASSERT_X(remoteAircraft.getCallsign() == aircraftModel.getCallsign(), Q_FUNC_INFO, "Mismatching callsigns");

        // decide CG
        const CLength cgModel = aircraftModel.getCG();
        const CLength cgSim = m_simulatorPlugin.second->getSimulatorCGPerModelString(aircraftModel.getModelString());
        const CSimulatorSettings simSettings = this->getSimulatorSettings();
        switch (simSettings.getCGSource())
        {
        case CSimulatorSettings::CGFromSimulatorOnly: aircraftModel.setCG(cgSim); break;
        case CSimulatorSettings::CGFromSimulatorFirst:
            if (!cgSim.isNull()) { aircraftModel.setCG(cgSim); }
            break;
        case CSimulatorSettings::CGFromDBFirst:
            if (cgModel.isNull()) { aircraftModel.setCG(cgSim); }
            break;
        // case CSimulatorSettings::CGFromDBOnly:
        default: break; // leave CG from model alone
        }

        const CLength overriddenCG =
            m_simulatorPlugin.second->overriddenCGorDefault(CLength::null(), aircraftModel.getModelString());
        if (!overriddenCG.isNull()) { aircraftModel.setCG(overriddenCG); }

        // model in provider
        this->updateAircraftModel(callsign, aircraftModel, this->identifier());

        const CSimulatedAircraft aircraftAfterModelApplied =
            this->getAircraftInRangeForCallsign(remoteAircraft.getCallsign());
        if (!aircraftAfterModelApplied.hasModelString())
        {
            if (!aircraftAfterModelApplied.hasCallsign()) { return; } // removed
            if (this->isAircraftInRange(aircraftAfterModelApplied.getCallsign()))
            {
                return;
            } // removed, but callsig, we did crosscheck

            // callsign, but no model string
            CLogMessage(this).error(u"Matching error for '%1', no model string")
                << aircraftAfterModelApplied.getCallsign().asString();

            CSimulatedAircraft brokenAircraft(aircraftAfterModelApplied);
            brokenAircraft.setEnabled(false);
            brokenAircraft.setRendered(false);
            CCallsign::addLogDetailsToList(
                pMatchingMessages, callsign,
                QStringLiteral("Cannot add remote aircraft, no model string: '%1'").arg(brokenAircraft.toQString()));
            emit this->aircraftRenderingChanged(brokenAircraft);
            return;
        }

        // here the model is added to the simulator
        m_simulatorPlugin.second->logicallyAddRemoteAircraft(aircraftAfterModelApplied);
        CCallsign::addLogDetailsToList(
            pMatchingMessages, callsign,
            QStringLiteral("Logically added remote aircraft: %1").arg(aircraftAfterModelApplied.toQString()));

        this->clearMatchingMessages(callsign);
        this->addMatchingMessages(callsign, matchingMessages);

        // done
        emit this->modelMatchingCompleted(aircraftAfterModelApplied);
    }

    void CContextSimulator::xCtxRemovedRemoteAircraft(const CCallsign &callsign)
    {
        if (!this->isSimulatorAvailable()) { return; }
        m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(callsign);
        m_failoverAddingCounts.remove(callsign);
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
                SWIFT_VERIFY_X(!simulatedAircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "Need callsign");
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

    void CContextSimulator::xCtxTextMessagesReceived(const network::CTextMessageList &textMessages)
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

    void CContextSimulator::xCtxChangedRemoteAircraftModel(const CSimulatedAircraft &aircraft,
                                                           const swift::misc::CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return; }
        if (!this->isSimulatorAvailable()) { return; }
        m_simulatorPlugin.second->changeRemoteAircraftModel(aircraft);
    }

    void CContextSimulator::xCtxChangedOwnAircraftModel(const CAircraftModel &aircraftModel,
                                                        const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return; }
        if (!this->isSimulatorAvailable()) { return; }

        emit this->ownAircraftModelChanged(aircraftModel);
    }

    void CContextSimulator::xCtxChangedRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
    {
        if (!this->isSimulatorAvailable()) { return; }
        m_simulatorPlugin.second->changeRemoteAircraftEnabled(aircraft);
    }

    void CContextSimulator::xCtxNetworkConnectionStatusChanged(const CConnectionStatus &from,
                                                               const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        SWIFT_VERIFY_X(this->getIContextNetwork(), Q_FUNC_INFO, "Missing network context");
        if (to.isConnected() && this->getIContextNetwork())
        {
            m_networkSessionId = this->getIContextNetwork()->getConnectedServer().getServerSessionId(false);
            if (m_simulatorPlugin.second) // check in case the plugin has been unloaded
            {
                m_simulatorPlugin.second->setFlightNetworkConnected(true);
            }
        }
        else if (to.isDisconnected())
        {
            m_networkSessionId.clear();
            m_aircraftMatcher.clearMatchingStatistics();
            m_matchingMessages.clear();
            m_failoverAddingCounts.clear();

            if (m_simulatorPlugin.second) // check in case the plugin has been unloaded
            {
                m_simulatorPlugin.second->removeAllRemoteAircraft(); // also removes aircraft
                m_simulatorPlugin.second->setFlightNetworkConnected(false);
            }
        }
    }

    void CContextSimulator::onAddingRemoteAircraftFailed(const CSimulatedAircraft &remoteAircraft, bool disabled,
                                                         bool requestFailover, const CStatusMessage &message)
    {
        if (!this->isSimulatorAvailable()) { return; }
        if (disabled) { m_aircraftMatcher.addingRemoteModelFailed(remoteAircraft); }

        const CCallsign cs = remoteAircraft.getCallsign();
        if (cs.isEmpty()) { return; }

        bool failover = requestFailover;
        if (requestFailover)
        {
            const CAircraftMatcherSetup setup = m_aircraftMatcher.getSetup();
            if (setup.doModelAddFailover())
            {
                const int trial = m_failoverAddingCounts.value(cs, 0);
                if (trial < MaxModelAddedFailoverTrials)
                {
                    m_failoverAddingCounts[cs] = trial + 1;
                    this->doMatchingAgain(cs); // has its own single shot logic
                }
                else
                {
                    failover = false;
                    CLogMessage(this).warning(u"Model for '%1' failed adding, tried %2 time(s) to resolve, giving up")
                        << cs.toQString(true) << (trial + 1);
                }
            }
        }
        emit this->addingRemoteModelFailed(remoteAircraft, disabled, failover, message);
    }

    void CContextSimulator::xCtxUpdateSimulatorCockpitFromContext(const CSimulatedAircraft &ownAircraft,
                                                                  const CIdentifier &originator)
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

    void CContextSimulator::xCtxNetworkRequestedNewAircraft(const CCallsign &callsign, const QString &aircraftIcao,
                                                            const QString &airlineIcao, const QString &livery)
    {
        if (m_networkSessionId.isEmpty()) { return; }
        m_aircraftMatcher.evaluateStatisticsEntry(m_networkSessionId, callsign, aircraftIcao, airlineIcao, livery);
    }

    void CContextSimulator::relayStatusMessageToSimulator(const swift::misc::CStatusMessage &message)
    {
        if (!this->isSimulatorAvailable()) { return; }
        if (!sApp || sApp->isShuttingDown()) { return; }
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

        this->stopSimulatorListeners();
        const QStringList enabledSimulators = m_enabledSimulators.getThreadLocal();
        const CSimulatorPluginInfoList allSimulators = m_plugins->getAvailableSimulatorPlugins();
        for (const CSimulatorPluginInfo &s : allSimulators)
        {
            if (enabledSimulators.contains(s.getIdentifier())) { startSimulatorPlugin(s); }
        }
    }

    CStatusMessageList CContextSimulator::getMatchingMessages(const CCallsign &callsign) const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
        return m_matchingMessages[callsign];
    }

    MatchingLog CContextSimulator::isMatchingMessagesEnabled() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return m_logMatchingMessages;
    }

    void CContextSimulator::enableMatchingMessages(MatchingLog enabled)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << matchingLogToString(enabled);
        }
        if (m_logMatchingMessages == enabled) { return; }
        m_logMatchingMessages = enabled;
        emit IContext::changedLogOrDebugSettings();
    }

    CMatchingStatistics CContextSimulator::getCurrentMatchingStatistics(bool missingOnly) const
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << missingOnly;
        }
        const CMatchingStatistics statistics = m_aircraftMatcher.getCurrentStatistics();
        return missingOnly ? statistics.findMissingOnly() : statistics;
    }

    void CContextSimulator::setMatchingSetup(const CAircraftMatcherSetup &setup)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << setup.toQString();
        }
        m_aircraftMatcher.setSetup(setup);
        const CStatusMessage msg = m_matchingSettings.setAndSave(setup);
        CLogMessage::preformatted(msg);
    }

    CAircraftMatcherSetup CContextSimulator::getMatchingSetup() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return m_aircraftMatcher.getSetup();
    }

    bool CContextSimulator::testRemoteAircraft(const CSimulatedAircraft &aircraft, bool add)
    {
        if (!m_simulatorPlugin.second || !m_simulatorPlugin.second->isConnected()) { return false; }
        bool added = add;
        if (add)
        {
            m_simulatorPlugin.second->setTestMode(true);
            added = m_simulatorPlugin.second->logicallyAddRemoteAircraft(aircraft);
        }
        else
        {
            m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(aircraft.getCallsign());
            m_simulatorPlugin.second->setTestMode(false); // AFTER we have removed it
        }
        return added;
    }

    bool CContextSimulator::testUpdateRemoteAircraft(const CCallsign &cs, const CAircraftSituation &situation,
                                                     const CAircraftParts &parts)
    {
        if (!m_simulatorPlugin.second || !m_simulatorPlugin.second->isConnected()) { return false; }
        CAircraftSituation s = situation; // make sure to have correct callsign
        s.setCallsign(cs);
        return m_simulatorPlugin.second->testSendSituationAndParts(cs, s, parts);
    }

    bool CContextSimulator::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        Q_UNUSED(originator)
        if (commandLine.isEmpty()) { return false; }
        CSimpleCommandParser parser({
            ".plugin", ".drv", ".driver", // forwarded to driver
            ".ris" // rendering interpolator setup
        });
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) { return false; }
        if (parser.matchesCommand("ris"))
        {
            CInterpolationAndRenderingSetupGlobal rs = this->getInterpolationAndRenderingSetupGlobal();
            const QString p1 = parser.part(1);
            if (!parser.hasPart(2)) { return false; }
            const bool on = stringToBool(parser.part(2));
            if (p1 == "debug") { rs.setSimulatorDebuggingMessages(on); }
            else if (p1 == "parts") { rs.setEnabledAircraftParts(on); }
            else { return false; }
            this->setInterpolationAndRenderingSetupGlobal(rs);
            CLogMessage(this, CLogCategories::cmdLine()).info(u"Setup is: '%1'") << rs.toQString(true);
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

    bool CContextSimulator::followAircraft(const CCallsign &callsign)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
        if (!m_simulatorPlugin.second) { return false; }
        return m_simulatorPlugin.second->followAircraft(callsign);
    }

    void CContextSimulator::recalculateAllAircraft()
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!m_simulatorPlugin.second) { return; }
        m_simulatorPlugin.second->recalculateAllAircraft();
    }

    bool CContextSimulator::resetToModelMatchingAircraft(const CCallsign &callsign)
    {
        CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
        if (aircraft.getCallsign() != callsign) { return false; } // not found
        if (!this->isSimulatorAvailable()) { return false; }

        m_simulatorPlugin.second->logicallyRemoveRemoteAircraft(callsign);
        aircraft.setModel(aircraft.getNetworkModel()); // like originally from network
        QPointer<CContextSimulator> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!sApp || sApp->isShuttingDown() || !myself) { return; }
            this->xCtxAddedRemoteAircraftReadyForModelMatching(aircraft);
        });
        return true;
    }

    bool CContextSimulator::requestElevationBySituation(const CAircraftSituation &situation)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << situation; }
        if (!m_simulatorPlugin.second || !m_simulatorPlugin.second->isConnected()) { return false; }
        return m_simulatorPlugin.second->requestElevationBySituation(situation);
    }

    CElevationPlane CContextSimulator::findClosestElevationWithinRange(const CCoordinateGeodetic &reference,
                                                                       const CLength &range) const
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug()
                << Q_FUNC_INFO << reference.convertToQString(true) << range;
        }
        if (!m_simulatorPlugin.second || !m_simulatorPlugin.second->isConnected()) { return CElevationPlane::null(); }
        return m_simulatorPlugin.second->findClosestElevationWithinRange(reference, range);
    }

    int CContextSimulator::doMatchingsAgain()
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();
        if (callsigns.isEmpty()) { return 0; }
        int delayMs = 25;
        QPointer<CContextSimulator> myself(this);
        for (const CCallsign &cs : callsigns)
        {
            QTimer::singleShot(delayMs, this, [=] {
                if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                this->doMatchingAgain(cs);
            });
            delayMs += 25;
        }
        return callsigns.size();
    }

    bool CContextSimulator::doMatchingAgain(const CCallsign &callsign)
    {
        if (isDebugEnabled())
        {
            CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << callsign.asString();
        }
        if (!this->isAircraftInRange(callsign)) { return false; }
        if (!this->isSimulatorAvailable()) { return false; }

        QPointer<CContextSimulator> myself(this);
        QTimer::singleShot(2500, this, [=] {
            if (!sApp || sApp->isShuttingDown() || !myself) { return; }
            const CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
            if (!aircraft.hasCallsign()) { return; } // no longer valid
            CSimulatedAircraft resetAircraft(aircraft);
            resetAircraft.resetToNetworkModel();
            resetAircraft.setEnabled(true);
            this->xCtxAddedRemoteAircraftReadyForModelMatching(resetAircraft);
        });
        return true;
    }

    void CContextSimulator::onSimulatorStarted(const CSimulatorPluginInfo &info)
    {
        this->stopSimulatorListeners();
        this->loadSimulatorPlugin(info);

        // if we have enabled messages, we will disable if size getting too high
        if (m_logMatchingMessages)
        {
            const QPointer<CContextSimulator> myself(this);
            QTimer::singleShot(5000, this, [=] {
                if (!myself) { return; }
                if (m_aircraftMatcher.getModelSetCount() > MatchingLogMaxModelSetSize)
                {
                    const MatchingLog log = CBuildConfig::isDebugBuild() ? MatchingLogAll : MatchingLogSimplified;
                    this->enableMatchingMessages(log);
                }
            });
        }
    }

    void CContextSimulator::onOwnSimulatorModelChanged(const CAircraftModel &model)
    {
        CAircraftModel lookupModel(model);
        if (!lookupModel.isLoadedFromDb()) { lookupModel = this->reverseLookupModel(model); }
        emit this->ownAircraftModelChanged(lookupModel);
    }

    void CContextSimulator::stopSimulatorListeners()
    {
        for (const CSimulatorPluginInfo &info : getAvailableSimulatorPlugins())
        {
            ISimulatorListener *listener = m_plugins->getListener(info.getIdentifier());
            if (listener)
            {
                const bool s = QMetaObject::invokeMethod(listener, &ISimulatorListener::stop);
                Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot invoke stop");
                Q_UNUSED(s)
            }
        }
    }

    void CContextSimulator::addMatchingMessages(const CCallsign &callsign, const CStatusMessageList &messages)
    {
        if (callsign.isEmpty()) { return; }
        if (messages.isEmpty()) { return; }
        if (!m_logMatchingMessages) { return; }
        if (m_matchingMessages.contains(callsign))
        {
            CStatusMessageList &msgs = m_matchingMessages[callsign];
            msgs.push_back(messages);
        }
        else { m_matchingMessages.insert(callsign, messages); }
    }

    void CContextSimulator::clearMatchingMessages(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        m_matchingMessages.remove(callsign);
    }

    CAircraftModel CContextSimulator::reverseLookupModel(const CAircraftModel &model)
    {
        bool modified = false;
        const CAircraftModel reverseModel =
            CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
        return reverseModel;
    }

    void CContextSimulator::initByLastUsedModelSet()
    {
        // no models in matcher, but in cache, we can set them as default
        const CSimulatorInfo simulator(m_modelSetSimulator.get());
        CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        const CAircraftModelList models(this->getModelSet()); // synced
        CLogMessage(this).info(u"Init aircraft matcher with %1 models from set for '%2'")
            << models.size() << simulator.toQString();
        m_aircraftMatcher.setModelSet(models, simulator, false);
    }
} // namespace swift::core::context
