// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/corefacade.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/sharedstate/datalinkdbus.h"
#include "blackmisc/loghistory.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/registermetadata.h"
#include "blackcore/airspacemonitor.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/identifier.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#include <QDBusConnection>
#include <QMap>
#include <QObject>
#include <QStringBuilder>
#include <QString>
#include <QElapsedTimer>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Data;
using namespace BlackCore::Context;

namespace BlackCore
{
    CCoreFacade::CCoreFacade(const CCoreFacadeConfig &config, QObject *parent) : QObject(parent), m_config(config)
    {
        this->init();
    }

    CStatusMessage CCoreFacade::tryToReconnectWithDBus()
    {
        if (m_shuttingDown) { return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Shutdown"); }
        if (!m_config.requiresDBusConnection()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Not DBus based"); }
        const QString dBusAddress = this->getDBusAddress();
        if (dBusAddress.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Not DBus based, no address"); }
        QString connectMsg;
        if (!CContextApplicationProxy::isContextResponsive(dBusAddress, connectMsg))
        {
            return CStatusMessage(this, CStatusMessage::SeverityError,
                                  u"Cannot connect DBus at '" % dBusAddress % u"', reason: " % connectMsg);
        }

        // re-init
        m_initalized = false;
        this->init();

        // success
        return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Re-initialized via '%1'") << dBusAddress;
    }

    void CCoreFacade::init()
    {
        if (m_initalized || m_shuttingDown) { return; }

        QMap<QString, qint64> times;
        QElapsedTimer time;
        CCoreFacade::registerMetadata();

        // either use explicit setting or last value
        const QString dbusAddress = this->getDBusAddress();
        m_launcherSetup.setProperty(CLauncherSetup::IndexDBusAddress, dbusAddress);

        // DBus
        time.start();
        if (m_config.requiresDBusSever()) { this->initDBusServer(dbusAddress); }
        if (m_config.requiresDBusConnection())
        {
            this->initDBusConnection(dbusAddress);
            if (!m_dbusConnection.isConnected())
            {
                const QString e = m_dbusConnection.lastError().message();
                BLACK_VERIFY_X(false, "CRuntime::init DBus problem", e.toUtf8().constData());
                CLogMessage(this).error(u"DBus connection failed: '%1'") << e;
                return;
            }
        }
        times.insert("DBus", time.restart());

        // shared state infrastructure
        m_dataLinkDBus = new SharedState::CDataLinkDBus(this);
        switch (m_config.getModeApplication())
        {
        case CCoreFacadeConfig::NotUsed:
        case CCoreFacadeConfig::Local:
            m_dataLinkDBus->initializeLocal(nullptr);
            break;
        case CCoreFacadeConfig::LocalInDBusServer:
            m_dataLinkDBus->initializeLocal(m_dbusServer);
            break;
        case CCoreFacadeConfig::Remote:
            m_dataLinkDBus->initializeRemote(m_dbusConnection, CDBusServer::coreServiceName(m_dbusConnection));
            break;
        default:
            qFatal("Invalid application context mode");
        }

        // shared log history
        m_logHistorySource = new CLogHistorySource(this);
        m_logHistorySource->initialize(m_dataLinkDBus);
        if (m_config.hasLocalCore())
        {
            m_logHistory = new CLogHistory(this);
            m_logHistory->initialize(m_dataLinkDBus);
        }

        if (m_config.all(CCoreFacadeConfig::NotUsed))
        {
            m_initalized = true;
            return;
        }

        // contexts
        if (m_contextApplication) { m_contextApplication->deleteLater(); }
        m_contextApplication = IContextApplication::create(this, m_config.getModeApplication(), m_dbusServer, m_dbusConnection);
        times.insert("Application", time.restart());

        if (m_contextAudio) { m_contextAudio->deleteLater(); }
        m_contextAudio = qobject_cast<CContextAudioBase *>(IContextAudio::create(this, m_config.getModeAudio(), m_dbusServer, m_dbusConnection));
        times.insert("Audio", time.restart());

        if (m_contextOwnAircraft) { m_contextOwnAircraft->deleteLater(); }
        m_contextOwnAircraft = IContextOwnAircraft::create(this, m_config.getModeOwnAircraft(), m_dbusServer, m_dbusConnection);
        times.insert("Own aircraft", time.restart());

        if (m_contextSimulator) { m_contextSimulator->deleteLater(); }
        m_contextSimulator = IContextSimulator::create(this, m_config.getModeSimulator(), m_dbusServer, m_dbusConnection);
        times.insert("Simulator", time.restart());

        // depends on own aircraft and simulator context, which is bad style
        if (m_contextNetwork) { m_contextNetwork->deleteLater(); }
        m_contextNetwork = IContextNetwork::create(this, m_config.getModeNetwork(), m_dbusServer, m_dbusConnection);
        times.insert("Network", time.restart());

        // checks --------------
        // 1. own aircraft and simulator should reside in same location
        Q_ASSERT(!m_contextSimulator || (m_contextOwnAircraft->isUsingImplementingObject() == m_contextSimulator->isUsingImplementingObject()));

        // 2. own aircraft and network should reside in same location
        Q_ASSERT(!m_contextNetwork || (m_contextOwnAircraft->isUsingImplementingObject() == m_contextNetwork->isUsingImplementingObject()));

        // post inits, wiring things among context (e.g. signal slots)
        time.restart();
        this->initPostSetup(times);
        times.insert("Post setup", time.restart());
        CLogMessage(this).info(u"Init times: %1") << qmapToString(times);

        // flag
        m_initalized = true;
    }

    void CCoreFacade::registerMetadata()
    {
        BlackMisc::registerMetadata();
        BlackCore::registerMetadata();
    }

    bool CCoreFacade::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        bool handled = false;
        // audio can be empty depending on whre it runs
        if (this->getIContextAudio() && !this->getIContextAudio()->isEmptyObject()) { handled = handled || this->getIContextAudio()->parseCommandLine(commandLine, originator); }
        if (this->getIContextNetwork()) { handled = handled || this->getIContextNetwork()->parseCommandLine(commandLine, originator); }
        if (this->getIContextOwnAircraft()) { handled = handled || this->getIContextOwnAircraft()->parseCommandLine(commandLine, originator); }
        if (this->getIContextSimulator()) { handled = handled || this->getIContextSimulator()->parseCommandLine(commandLine, originator); }
        return handled;
    }

    void CCoreFacade::initDBusServer(const QString &dBusAddress)
    {
        Q_ASSERT(!dBusAddress.isEmpty());
        if (m_dbusServer) { m_dbusServer->deleteLater(); } // delete if there was an existing one
        m_dbusServer = new CDBusServer(dBusAddress, this);
        CLogMessage(this).info(u"DBus server on address: '%1'") << dBusAddress;
    }

    void CCoreFacade::initPostSetup(QMap<QString, qint64> &times)
    {
        bool c = false;
        Q_UNUSED(c) // for release version
        QElapsedTimer time;
        time.start();

        times.insert("Post setup, connects first", time.restart());

        // local simulator?
        if (m_contextSimulator && m_contextSimulator->isUsingImplementingObject())
        {
            // only connect if network runs locally, no round trips
            // remark: from a design perspective it would be nice if those could be avoided (seperation of concerns)
            //         those connects here reprent cross context dependencies
            if (m_contextNetwork && m_contextNetwork->isUsingImplementingObject())
            {
                Q_ASSERT_X(this->getCContextNetwork(), Q_FUNC_INFO, "No local network object");
                Q_ASSERT_X(this->getCContextNetwork()->airspace(), Q_FUNC_INFO, "No airspace object");

                c = connect(m_contextNetwork, &IContextNetwork::textMessagesReceived,
                            this->getCContextSimulator(), &CContextSimulator::xCtxTextMessagesReceived, Qt::QueuedConnection);
                Q_ASSERT(c);

                // use readyForModelMatching instead of CAirspaceMonitor::addedAircraft, as it contains client information
                // ready for model matching is sent delayed when all information are available
                c = connect(m_contextNetwork, &IContextNetwork::readyForModelMatching,
                            this->getCContextSimulator(), &CContextSimulator::xCtxAddedRemoteAircraftReadyForModelMatching, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::removedAircraft,
                            this->getCContextSimulator(), &CContextSimulator::xCtxRemovedRemoteAircraft, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::changedRemoteAircraftModel,
                            this->getCContextSimulator(), &CContextSimulator::xCtxChangedRemoteAircraftModel, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::changedRemoteAircraftEnabled,
                            this->getCContextSimulator(), &CContextSimulator::xCtxChangedRemoteAircraftEnabled, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::connectionStatusChanged,
                            this->getCContextSimulator(), &CContextSimulator::xCtxNetworkConnectionStatusChanged, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(this->getCContextNetwork()->airspace(), &CAirspaceMonitor::requestedNewAircraft,
                            this->getCContextSimulator(), &CContextSimulator::xCtxNetworkRequestedNewAircraft, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::renderRestrictionsChanged,
                            this->getCContextNetwork(), &CContextNetwork::xCtxSimulatorRenderRestrictionsChanged, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::simulatorStatusChanged,
                            this->getCContextNetwork(), &CContextNetwork::xCtxSimulatorStatusChanged, Qt::QueuedConnection);
                Q_ASSERT(c);

                // set provider
                this->getCContextNetwork()->setSimulationEnvironmentProvider(this->getCContextSimulator()->simulator());
            }

            // only if own aircraft runs locally
            if (m_contextOwnAircraft && m_contextOwnAircraft->isUsingImplementingObject())
            {
                c = connect(m_contextOwnAircraft, &IContextOwnAircraft::changedAircraftCockpit,
                            this->getCContextSimulator(), &CContextSimulator::xCtxUpdateSimulatorCockpitFromContext);
                Q_ASSERT(c);
                c = connect(m_contextOwnAircraft, &IContextOwnAircraft::changedSelcal,
                            this->getCContextSimulator(), &CContextSimulator::xCtxUpdateSimulatorSelcalFromContext);
                Q_ASSERT(c);

                // relay changed aircraft to own aircraft provider but with identifier
                // identifier is needed because own aircraft context also reports changed aircraft to xCtxChangedOwnAircraftModel
                // and we avoid roundtrips
                c = connect(this->getCContextSimulator(), &CContextSimulator::ownAircraftModelChanged,
                            this->getCContextOwnAircraft(), [=](const CAircraftModel &changedModel) {
                                if (!this->getIContextOwnAircraft()) { return; }
                                if (!this->getCContextSimulator()) { return; }
                                this->getCContextOwnAircraft()->xCtxChangedSimulatorModel(changedModel, this->getCContextSimulator()->identifier());
                            });

                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::simulatorStatusChanged,
                            this->getCContextOwnAircraft(), &CContextOwnAircraft::xCtxChangedSimulatorStatus);
                Q_ASSERT(c);

                // this is used if the value in own aircraft is changed, to callback simulator
                c = connect(this->getCContextOwnAircraft(), &CContextOwnAircraft::ps_changedModel,
                            this->getCContextSimulator(), &CContextSimulator::xCtxChangedOwnAircraftModel);
                Q_ASSERT(c);
            }

            // times
            times.insert("Post setup, sim.connects", time.restart());
        }

        // only where network and(!) own aircraft run locally
        // -> in the core or an all local implementation
        if (m_contextNetwork && m_contextOwnAircraft && m_contextNetwork->isUsingImplementingObject() && m_contextOwnAircraft->isUsingImplementingObject())
        {
            c = connect(m_contextNetwork, &IContextNetwork::atcStationDisconnected,
                        this->getCContextOwnAircraft(), &CContextOwnAircraft::xCtxAtcStationDisconnected);
            Q_ASSERT(c);
            times.insert("Post setup, connects network", time.restart());
        }

        // connection status of network changed
        // with AFV no longer use m_contextAudio->isUsingImplementingObject() as audio can run on both sides
        if (this->getCContextAudioBase() && m_contextNetwork)
        {
            Q_ASSERT(m_contextApplication);
            c = connect(m_contextNetwork, &IContextNetwork::connectionStatusChanged,
                        this->getCContextAudioBase(), &CContextAudio::xCtxNetworkConnectionStatusChanged, Qt::QueuedConnection);
            Q_ASSERT(c);
            times.insert("Post setup, connects audio", time.restart());
        }
    }

    QString CCoreFacade::getDBusAddress() const
    {
        QString dbusAddress;
        if (m_config.hasDBusAddress())
        {
            dbusAddress = m_config.getDBusAddress();
        }
        else
        {
            const CLauncherSetup setup = m_launcherSetup.get();
            dbusAddress = setup.getDBusAddress();
        }
        return dbusAddress;
    }

    void CCoreFacade::gracefulShutdown()
    {
        if (!m_initalized) { return; }
        if (m_shuttingDown) { return; }
        m_shuttingDown = true;

        // disable all signals towards runtime
        disconnect(this);

        // tear down shared state infrastructure
        delete m_logHistory;
        m_logHistory = nullptr;
        delete m_logHistorySource;
        m_logHistorySource = nullptr;
        delete m_dataLinkDBus;
        m_dataLinkDBus = nullptr;

        // unregister all from DBus
        if (m_dbusServer) { m_dbusServer->removeAllObjects(); }

        // handle contexts

        if (this->getCContextAudioBase())
        {
            // there is no empty audio context since AFV
            disconnect(this->getCContextAudioBase());
            this->getCContextAudioBase()->gracefulShutdown();
            this->getIContextAudio()->deleteLater();
            m_contextAudio = nullptr;
        }

        // log off from network, if connected
        if (this->getIContextNetwork())
        {
            disconnect(this->getIContextNetwork());
            this->getIContextNetwork()->disconnectFromNetwork();
            if (m_contextNetwork->isUsingImplementingObject())
            {
                this->getCContextNetwork()->gracefulShutdown(); // for threads
            }
            this->getIContextNetwork()->deleteLater();
            // replace by dummy object avoiding nullptr issues during shutdown phase
            QDBusConnection defaultConnection("default");
            m_contextNetwork = IContextNetwork::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextSimulator())
        {
            disconnect(this->getIContextSimulator());
            if (this->getIContextSimulator()->isUsingImplementingObject())
            {
                // shutdown the plugins
                this->getCContextSimulator()->gracefulShutdown();
            }
            this->getIContextSimulator()->deleteLater();
            QDBusConnection defaultConnection("default");
            m_contextSimulator = IContextSimulator::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextOwnAircraft())
        {
            disconnect(this->getIContextOwnAircraft());
            this->getIContextOwnAircraft()->deleteLater();
            QDBusConnection defaultConnection("default");
            m_contextOwnAircraft = IContextOwnAircraft::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextApplication())
        {
            disconnect(this->getIContextApplication());
            this->getIContextApplication()->deleteLater();
            QDBusConnection defaultConnection("default");
            m_contextApplication = IContextApplication::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }
    }

    void CCoreFacade::initDBusConnection(const QString &address)
    {
        if (m_initDBusConnection) { return; }
        if (address.isEmpty() || address == CDBusServer::sessionBusAddress())
        {
            QDBusConnection::disconnectFromBus(m_dbusConnection.name());
            m_dbusConnection = QDBusConnection::sessionBus();
        }
        else if (address == CDBusServer::systemBusAddress())
        {
            QDBusConnection::disconnectFromBus(m_dbusConnection.name());
            m_dbusConnection = QDBusConnection::systemBus();
        }
        else
        {
            const QString name(CDBusServer::p2pConnectionName() + " " + address);
            QDBusConnection::disconnectFromPeer(name);
            m_dbusConnection = QDBusConnection::connectToPeer(address, name);
        }
    }

    const IContextApplication *CCoreFacade::getIContextApplication() const
    {
        return m_contextApplication;
    }

    IContextApplication *CCoreFacade::getIContextApplication()
    {
        return m_contextApplication;
    }

    IContextAudio *CCoreFacade::getIContextAudio()
    {
        return m_contextAudio;
    }

    const IContextAudio *CCoreFacade::getIContextAudio() const
    {
        return m_contextAudio;
    }

    CContextAudioBase *CCoreFacade::getCContextAudioBase()
    {
        return m_contextAudio;
    }

    const CContextAudioBase *CCoreFacade::getCContextAudioBase() const
    {
        return m_contextAudio;
    }

    IContextNetwork *CCoreFacade::getIContextNetwork()
    {
        return m_contextNetwork;
    }

    const IContextNetwork *CCoreFacade::getIContextNetwork() const
    {
        return m_contextNetwork;
    }

    IContextOwnAircraft *CCoreFacade::getIContextOwnAircraft()
    {
        return m_contextOwnAircraft;
    }

    const IContextOwnAircraft *CCoreFacade::getIContextOwnAircraft() const
    {
        return m_contextOwnAircraft;
    }

    const IContextSimulator *CCoreFacade::getIContextSimulator() const
    {
        return m_contextSimulator;
    }

    IContextSimulator *CCoreFacade::getIContextSimulator()
    {
        return m_contextSimulator;
    }

    CContextAudio *CCoreFacade::getCContextAudio()
    {
        Q_ASSERT_X(m_contextAudio && m_contextAudio->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(m_contextAudio);
    }

    const CContextAudio *CCoreFacade::getCContextAudio() const
    {
        Q_ASSERT_X(m_contextAudio && m_contextAudio->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(m_contextAudio);
    }

    CContextApplication *CCoreFacade::getCContextApplication()
    {
        Q_ASSERT_X(m_contextApplication && m_contextApplication->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(m_contextApplication);
    }

    const CContextApplication *CCoreFacade::getCContextApplication() const
    {
        Q_ASSERT_X(m_contextApplication && m_contextApplication->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(m_contextApplication);
    }

    CContextNetwork *CCoreFacade::getCContextNetwork()
    {
        Q_ASSERT_X(m_contextNetwork && m_contextNetwork->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(m_contextNetwork);
    }

    const CContextNetwork *CCoreFacade::getCContextNetwork() const
    {
        Q_ASSERT_X(m_contextNetwork && m_contextNetwork->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(m_contextNetwork);
    }

    CContextOwnAircraft *CCoreFacade::getCContextOwnAircraft()
    {
        Q_ASSERT_X(m_contextOwnAircraft && m_contextOwnAircraft->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(m_contextOwnAircraft);
    }

    const CContextOwnAircraft *CCoreFacade::getCContextOwnAircraft() const
    {
        Q_ASSERT_X(m_contextOwnAircraft && m_contextOwnAircraft->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(m_contextOwnAircraft);
    }

    CContextSimulator *CCoreFacade::getCContextSimulator()
    {
        Q_ASSERT_X(m_contextSimulator && m_contextSimulator->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(m_contextSimulator);
    }

    const CContextSimulator *CCoreFacade::getCContextSimulator() const
    {
        Q_ASSERT_X(m_contextSimulator && m_contextSimulator->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(m_contextSimulator);
    }
} // namespace
