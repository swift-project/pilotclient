/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/registermetadata.h"
#include "blackcore/airspacemonitor.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/identifier.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#include <stdbool.h>
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusError>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTime>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Data;
using namespace BlackCore::Context;

namespace BlackCore
{
    CCoreFacade::CCoreFacade(const CCoreFacadeConfig &config, QObject *parent) :
        QObject(parent), m_config(config)
    {
        this->init();
    }

    CStatusMessage CCoreFacade::tryToReconnectWithDBus()
    {
        if (m_shuttingDown) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "Shutdown"); }
        if (!m_config.requiresDBusConnection()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "Not DBus based"); }
        const QString dBusAddress = this->getDBusAddress();
        if (dBusAddress.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "Not DBus based, no address"); }
        QString connectMsg;
        if (!CContextApplicationProxy::isContextResponsive(dBusAddress, connectMsg))
        {
            return CStatusMessage(this, CStatusMessage::SeverityError,
                                  "Cannot connect DBus at '" + dBusAddress + "', reason: " + connectMsg);
        }

        // re-init
        m_initalized = false;
        this->init();

        // success
        return CStatusMessage(this, CStatusMessage::SeverityInfo, "Re-initialized via '%1'") << dBusAddress;
    }

    void CCoreFacade::init()
    {
        if (m_initalized || m_shuttingDown) { return; }

        QMap<QString, int> times;
        QTime time;
        CCoreFacade::registerMetadata();

        // either use explicit setting or last value
        const QString dbusAddress = this->getDBusAddress();
        m_launcherSetup.setProperty(CLauncherSetup::IndexDBusAddress, dbusAddress);

        // DBus
        if (m_config.requiresDBusSever()) { this->initDBusServer(dbusAddress); }
        if (m_config.requiresDBusConnection())
        {
            this->initDBusConnection(dbusAddress);
            if (!m_dbusConnection.isConnected())
            {
                const QString e = m_dbusConnection.lastError().message();
                BLACK_VERIFY_X(false, "CRuntime::init DBus problem", e.toUtf8().constData());
                CLogMessage(this).error("DBus connection failed: '%1'") << e;
                return;
            }
        }
        times.insert("DBus", time.restart());

        // contexts
        if (m_contextApplication) { m_contextApplication->deleteLater(); }
        m_contextApplication = IContextApplication::create(this, m_config.getModeApplication(), m_dbusServer, m_dbusConnection);
        times.insert("Application", time.restart());

        if (m_contextAudio) { m_contextAudio->deleteLater(); }
        m_contextAudio = IContextAudio::create(this, m_config.getModeAudio(), m_dbusServer, m_dbusConnection);
        times.insert("Audio", time.restart());

        if (m_contextOwnAircraft) { m_contextOwnAircraft->deleteLater(); }
        m_contextOwnAircraft = IContextOwnAircraft::create(this, m_config.getModeOwnAircraft(), m_dbusServer, m_dbusConnection);
        times.insert("Own aircraft", time.restart());

        if (m_contextNetwork) { m_contextNetwork->deleteLater(); }
        m_contextNetwork = IContextNetwork::create(this, m_config.getModeNetwork(), m_dbusServer, m_dbusConnection);
        times.insert("Network", time.restart());

        if (m_contextSimulator) { m_contextSimulator->deleteLater(); }
        m_contextSimulator = IContextSimulator::create(this, m_config.getModeSimulator(), m_dbusServer, m_dbusConnection);
        times.insert("Simulator", time.restart());

        // checks --------------
        // 1. own aircraft and simulator should reside in same location
        Q_ASSERT(!m_contextSimulator || (m_contextOwnAircraft->isUsingImplementingObject() == m_contextSimulator->isUsingImplementingObject()));

        // 2. own aircraft and network should reside in same location
        Q_ASSERT(!m_contextNetwork || (m_contextOwnAircraft->isUsingImplementingObject() == m_contextNetwork->isUsingImplementingObject()));

        // post inits, wiring things among context (e.g. signal slots)
        time.restart();
        this->initPostSetup(times);
        times.insert("Post setup", time.restart());
        CLogMessage(this).info("Init times: %1") << qmapToString(times);

        // flag
        m_initalized = true;
    }

    bool CCoreFacade::hasRemoteApplicationContext() const
    {
        Q_ASSERT(m_contextApplication);
        return !m_contextApplication->isUsingImplementingObject();
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
        CLogMessage(this).info("DBus server on address: '%1'") << dBusAddress;
    }

    void CCoreFacade::initPostSetup(QMap<QString, int> &times)
    {
        bool c = false;
        Q_UNUSED(c); // for release version
        QTime time;
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
                            this->getCContextSimulator(), &CContextSimulator::ps_textMessagesReceived);
                Q_ASSERT(c);

                // use readyForModelMatching instead of CAirspaceMonitor::addedAircraft, as it contains client information
                // ready for model matching is sent delayed when all information are available
                c = connect(m_contextNetwork, &IContextNetwork::readyForModelMatching,
                            this->getCContextSimulator(), &CContextSimulator::ps_addedRemoteAircraft);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::removedAircraft,
                            this->getCContextSimulator(), &CContextSimulator::ps_removedRemoteAircraft);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::changedRemoteAircraftModel,
                            this->getCContextSimulator(), &CContextSimulator::ps_changedRemoteAircraftModel);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::changedRemoteAircraftEnabled,
                            this->getCContextSimulator(), &CContextSimulator::ps_changedRemoteAircraftEnabled);
                Q_ASSERT(c);
                c = connect(m_contextNetwork, &IContextNetwork::connectionStatusChanged,
                            this->getCContextSimulator(), &CContextSimulator::ps_networkConnectionStatusChanged);
                Q_ASSERT(c);
                c = connect(this->getCContextNetwork()->airspace(), &CAirspaceMonitor::requestedNewAircraft,
                            this->getCContextSimulator(), &CContextSimulator::ps_networkRequestedNewAircraft);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::renderRestrictionsChanged,
                            this->getCContextNetwork(), &CContextNetwork::simulatorRenderRestrictionsChanged);
                Q_ASSERT(c);
            }

            // only if own aircraft runs locally
            if (m_contextOwnAircraft && m_contextOwnAircraft->isUsingImplementingObject())
            {
                c = connect(m_contextOwnAircraft, &IContextOwnAircraft::changedAircraftCockpit,
                            this->getCContextSimulator(), &CContextSimulator::ps_updateSimulatorCockpitFromContext);
                Q_ASSERT(c);
                c = connect(m_contextOwnAircraft, &IContextOwnAircraft::changedSelcal,
                            this->getCContextSimulator(), &CContextSimulator::ps_updateSimulatorSelcalFromContext);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::ownAircraftModelChanged,
                            this->getCContextOwnAircraft(), &CContextOwnAircraft::changedSimulatorModel);
                Q_ASSERT(c);
            }

            // times
            times.insert("Post setup, sim.connects", time.restart());
        }

        // only where network and(!) own aircraft run locally
        // -> in the core or an all local implementation
        if (m_contextNetwork && m_contextOwnAircraft && m_contextNetwork->isUsingImplementingObject() && m_contextOwnAircraft->isUsingImplementingObject())
        {
            c = connect(m_contextNetwork, &IContextNetwork::changedAtcStationOnlineConnectionStatus,
                        this->getCContextOwnAircraft(),  &CContextOwnAircraft::changedAtcStationOnlineConnectionStatus);
            Q_ASSERT(c);
            times.insert("Post setup, connects network", time.restart());
        }

        // fake signals to work around setting values in audio context for local audio with remote core
        if (m_contextAudio && m_contextAudio->isUsingImplementingObject())
        {
            Q_ASSERT(m_contextApplication);
            Q_ASSERT(m_contextOwnAircraft);
            c = connect(m_contextApplication, &IContextApplication::fakedSetComVoiceRoom,
                        this->getCContextAudio(),  &CContextAudio::setComVoiceRooms);
            Q_ASSERT(c);
            c = connect(m_contextOwnAircraft, &IContextOwnAircraft::changedCallsign, this->getCContextAudio(), &IContextAudio::setOwnCallsignForRooms);
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

        // unregister all from DBus
        if (m_dbusServer) { m_dbusServer->removeAllObjects(); }

        // handle contexts
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

        if (this->getIContextAudio())
        {
            disconnect(this->getIContextAudio());
            this->getIContextAudio()->deleteLater();
            // replace by dummy object avoiding nullptr issues during shutdown phase
            QDBusConnection defaultConnection("default");
            m_contextAudio = IContextAudio::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
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
