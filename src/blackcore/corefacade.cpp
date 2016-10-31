/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/registermetadata.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/identifier.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"

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
using namespace BlackCore::Context;

namespace BlackCore
{
    CCoreFacade::CCoreFacade(const CCoreFacadeConfig &config, QObject *parent) : QObject(parent)
    {
        this->init(config);
    }

    void CCoreFacade::init(const CCoreFacadeConfig &config)
    {
        if (m_init) { return; }

        QMap<QString, int> times;
        QTime time;
        registerMetadata();

        // either use explicit setting or last value
        QString dbusAddress;
        if (config.hasDBusAddress())
        {
            dbusAddress = config.getDBusAddress();
            m_dbusServerAddress.set(dbusAddress);
        }
        else
        {
            dbusAddress = m_dbusServerAddress.getThreadLocal();
        }

        // DBus
        if (config.requiresDBusSever()) { this->initDBusServer(dbusAddress); }
        if (config.requiresDBusConnection())
        {
            this->initDBusConnection(dbusAddress);
            if (!this->m_dbusConnection.isConnected())
            {
                QString notConnected("DBus connection failed: ");
                const QString e = this->m_dbusConnection.lastError().message();
                if (!e.isEmpty()) { notConnected.append(e); }
                Q_ASSERT_X(false, "CRuntime::init", notConnected.toUtf8().constData());
                CLogMessage(this).error(notConnected);
            }
        }
        times.insert("DBus", time.restart());

        // contexts
        this->m_contextApplication = IContextApplication::create(this, config.getModeApplication(), this->m_dbusServer, this->m_dbusConnection);
        times.insert("Application", time.restart());

        this->m_contextAudio = IContextAudio::create(this, config.getModeAudio(), this->m_dbusServer, this->m_dbusConnection);
        times.insert("Audio", time.restart());

        this->m_contextOwnAircraft = IContextOwnAircraft::create(this, config.getModeOwnAircraft(), this->m_dbusServer, this->m_dbusConnection);
        times.insert("Own aircraft", time.restart());

        this->m_contextNetwork = IContextNetwork::create(this, config.getModeNetwork(), this->m_dbusServer, this->m_dbusConnection);
        times.insert("Network", time.restart());

        this->m_contextSimulator = IContextSimulator::create(this, config.getModeSimulator(), this->m_dbusServer, this->m_dbusConnection);
        times.insert("Simulator", time.restart());

        // checks --------------
        // 1. own aircraft and simulator should reside in same location
        Q_ASSERT(!this->m_contextSimulator || (this->m_contextOwnAircraft->isUsingImplementingObject() == this->m_contextSimulator->isUsingImplementingObject()));

        // 2. own aircraft and network should reside in same location
        Q_ASSERT(!this->m_contextNetwork || (this->m_contextOwnAircraft->isUsingImplementingObject() == this->m_contextNetwork->isUsingImplementingObject()));

        // post inits, wiring things among context (e.g. signal slots)
        time.restart();
        this->initPostSetup(times);
        times.insert("Post setup", time.restart());
        CLogMessage(this).info("Init times: %1") << qmapToString(times);

        // flag
        m_init = true;
    }

    bool CCoreFacade::hasRemoteApplicationContext() const
    {
        Q_ASSERT(this->m_contextApplication);
        return !this->m_contextApplication->isUsingImplementingObject();
    }

    void CCoreFacade::registerMetadata()
    {
        BlackMisc::registerMetadata();
        BlackCore::registerMetadata();
    }

    bool CCoreFacade::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        bool handled = false;
        if (this->getIContextAudio()) { handled = handled || this->getIContextAudio()->parseCommandLine(commandLine, originator); }
        if (this->getIContextNetwork()) { handled = handled || this->getIContextNetwork()->parseCommandLine(commandLine, originator); }
        if (this->getIContextOwnAircraft()) { handled = handled || this->getIContextOwnAircraft()->parseCommandLine(commandLine, originator); }
        return handled;
    }

    void CCoreFacade::initDBusServer(const QString &dBusAddress)
    {
        if (this->m_dbusServer) { return; }
        Q_ASSERT(!dBusAddress.isEmpty());
        this->m_dbusServer = new CDBusServer(dBusAddress, this);
        CLogMessage(this).info("DBus server on address: %1") << dBusAddress;
    }

    void CCoreFacade::initPostSetup(QMap<QString, int> &times)
    {
        bool c = false;
        Q_UNUSED(c); // for release version
        QTime time;
        time.start();

        times.insert("Post setup, connects first", time.restart());

        // local simulator?
        if (this->m_contextSimulator && this->m_contextSimulator->isUsingImplementingObject())
        {
            // only connect if network runs locally, no round trips
            if (this->m_contextNetwork && this->m_contextNetwork->isUsingImplementingObject())
            {
                c = connect(this->m_contextNetwork, &IContextNetwork::textMessagesReceived,
                            this->getCContextSimulator(), &CContextSimulator::ps_textMessagesReceived);
                Q_ASSERT(c);

                // use readyForModelMatching instead of CAirspaceMonitor::addedAircraft, as it contains client information
                // ready for model matching is sent delayed when all information are available
                c = connect(this->m_contextNetwork, &IContextNetwork::readyForModelMatching,
                            this->getCContextSimulator(), &CContextSimulator::ps_addedRemoteAircraft);
                Q_ASSERT(c);
                c = connect(this->m_contextNetwork, &IContextNetwork::removedAircraft,
                            this->getCContextSimulator(), &CContextSimulator::ps_removedRemoteAircraft);
                Q_ASSERT(c);
                c = connect(this->m_contextNetwork, &IContextNetwork::changedRemoteAircraftModel,
                            this->getCContextSimulator(), &CContextSimulator::ps_changedRemoteAircraftModel);
                Q_ASSERT(c);
                c = connect(this->m_contextNetwork, &IContextNetwork::changedRemoteAircraftEnabled,
                            this->getCContextSimulator(), &CContextSimulator::ps_changedRemoteAircraftEnabled);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::renderRestrictionsChanged,
                            this->getCContextNetwork(), &CContextNetwork::ps_simulatorRenderRestrictionsChanged);
                Q_ASSERT(c);
            }

            // only if own aircraft runs locally
            if (this->m_contextOwnAircraft && this->m_contextOwnAircraft->isUsingImplementingObject())
            {
                c = connect(this->m_contextOwnAircraft, &IContextOwnAircraft::changedAircraftCockpit,
                            this->getCContextSimulator(), &CContextSimulator::ps_updateSimulatorCockpitFromContext);
                Q_ASSERT(c);
                c = connect(this->getCContextSimulator(), &CContextSimulator::ownAircraftModelChanged,
                            this->getCContextOwnAircraft(), &CContextOwnAircraft::ps_changedSimulatorModel);
                Q_ASSERT(c);
            }

            // times
            times.insert("Post setup, sim.connects", time.restart());
        }

        // only where network and(!) own aircraft run locally
        // -> in the core or an all local implementation
        if (this->m_contextNetwork && this->m_contextOwnAircraft && this->m_contextNetwork->isUsingImplementingObject() && this->m_contextOwnAircraft->isUsingImplementingObject())
        {
            c = this->connect(this->m_contextNetwork, &IContextNetwork::changedAtcStationOnlineConnectionStatus,
                              this->getCContextOwnAircraft(),  &CContextOwnAircraft::ps_changedAtcStationOnlineConnectionStatus);
            Q_ASSERT(c);
            times.insert("Post setup, connects network", time.restart());
        }

        // fake signals to work around setting values in audio context for local audio with remote core
        if (this->m_contextAudio && this->m_contextAudio->isUsingImplementingObject())
        {
            Q_ASSERT(this->m_contextApplication);
            Q_ASSERT(this->m_contextOwnAircraft);
            c = this->connect(this->m_contextApplication, &IContextApplication::fakedSetComVoiceRoom,
                              this->getCContextAudio(),  &CContextAudio::setComVoiceRooms);
            Q_ASSERT(c);
            c = this->connect(this->m_contextOwnAircraft, &IContextOwnAircraft::changedCallsign, this->getCContextAudio(), &IContextAudio::setOwnCallsignForRooms);
            Q_ASSERT(c);
            times.insert("Post setup, connects audio", time.restart());
        }
    }

    void CCoreFacade::gracefulShutdown()
    {
        if (!this->m_init) return;
        this->m_init = false;

        // disable all signals towards runtime
        disconnect(this);

        // unregister all from DBus
        if (this->m_dbusServer) { this->m_dbusServer->removeAllObjects(); }

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
            this->m_contextSimulator = IContextSimulator::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        // log off from network, if connected
        if (this->getIContextNetwork())
        {
            disconnect(this->getIContextNetwork());
            this->getIContextNetwork()->disconnectFromNetwork();
            if (this->m_contextNetwork->isUsingImplementingObject())
            {
                this->getCContextNetwork()->gracefulShutdown(); // for threads
            }
            this->getIContextNetwork()->deleteLater();
            // replace by dummy object avoiding nullptr issues during shutdown phase
            QDBusConnection defaultConnection("default");
            this->m_contextNetwork = IContextNetwork::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextAudio())
        {
            disconnect(this->getIContextAudio());
            this->getIContextAudio()->deleteLater();
            // replace by dummy object avoiding nullptr issues during shutdown phase
            QDBusConnection defaultConnection("default");
            this->m_contextAudio = IContextAudio::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextOwnAircraft())
        {
            disconnect(this->getIContextOwnAircraft());
            this->getIContextOwnAircraft()->deleteLater();
            QDBusConnection defaultConnection("default");
            this->m_contextOwnAircraft = IContextOwnAircraft::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }

        if (this->getIContextApplication())
        {
            disconnect(this->getIContextApplication());
            this->getIContextApplication()->deleteLater();
            QDBusConnection defaultConnection("default");
            this->m_contextApplication = IContextApplication::create(this, CCoreFacadeConfig::NotUsed, nullptr, defaultConnection);
        }
    }

    void CCoreFacade::initDBusConnection(const QString &address)
    {
        if (this->m_initDBusConnection) { return; }
        if (address.isEmpty() || address == CDBusServer::sessionBusAddress())
        {
            this->m_dbusConnection = QDBusConnection::sessionBus();
        }
        else if (address == CDBusServer::systemBusAddress())
        {
            this->m_dbusConnection = QDBusConnection::sessionBus();
        }
        else
        {
            this->m_dbusConnection = QDBusConnection::connectToPeer(address, "BlackBoxRuntime");
        }
    }

    const IContextApplication *CCoreFacade::getIContextApplication() const
    {
        return this->m_contextApplication;
    }

    IContextApplication *CCoreFacade::getIContextApplication()
    {
        return this->m_contextApplication;
    }

    IContextAudio *CCoreFacade::getIContextAudio()
    {
        return this->m_contextAudio;
    }

    const IContextAudio *CCoreFacade::getIContextAudio() const
    {
        return this->m_contextAudio;
    }

    IContextNetwork *CCoreFacade::getIContextNetwork()
    {
        return this->m_contextNetwork;
    }

    const IContextNetwork *CCoreFacade::getIContextNetwork() const
    {
        return this->m_contextNetwork;
    }

    IContextOwnAircraft *CCoreFacade::getIContextOwnAircraft()
    {
        return this->m_contextOwnAircraft;
    }

    const IContextOwnAircraft *CCoreFacade::getIContextOwnAircraft() const
    {
        return this->m_contextOwnAircraft;
    }

    const IContextSimulator *CCoreFacade::getIContextSimulator() const
    {
        return this->m_contextSimulator;
    }

    IContextSimulator *CCoreFacade::getIContextSimulator()
    {
        return this->m_contextSimulator;
    }

    CContextAudio *CCoreFacade::getCContextAudio()
    {
        Q_ASSERT_X(this->m_contextAudio && this->m_contextAudio->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    const CContextAudio *CCoreFacade::getCContextAudio() const
    {
        Q_ASSERT_X(this->m_contextAudio && this->m_contextAudio->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextApplication *CCoreFacade::getCContextApplication()
    {
        Q_ASSERT_X(this->m_contextApplication && this->m_contextApplication->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    const CContextApplication *CCoreFacade::getCContextApplication() const
    {
        Q_ASSERT_X(this->m_contextApplication && this->m_contextApplication->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextNetwork *CCoreFacade::getCContextNetwork()
    {
        Q_ASSERT_X(this->m_contextNetwork && this->m_contextNetwork->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(this->m_contextNetwork);
    }

    const CContextNetwork *CCoreFacade::getCContextNetwork() const
    {
        Q_ASSERT_X(this->m_contextNetwork && this->m_contextNetwork->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(this->m_contextNetwork);
    }

    CContextOwnAircraft *CCoreFacade::getCContextOwnAircraft()
    {
        Q_ASSERT_X(this->m_contextOwnAircraft && this->m_contextOwnAircraft->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(this->m_contextOwnAircraft);
    }

    const CContextOwnAircraft *CCoreFacade::getCContextOwnAircraft() const
    {
        Q_ASSERT_X(this->m_contextOwnAircraft && this->m_contextOwnAircraft->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(this->m_contextOwnAircraft);
    }

    CContextSimulator *CCoreFacade::getCContextSimulator()
    {
        Q_ASSERT_X(this->m_contextSimulator && this->m_contextSimulator->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    const CContextSimulator *CCoreFacade::getCContextSimulator() const
    {
        Q_ASSERT_X(this->m_contextSimulator && this->m_contextSimulator->isUsingImplementingObject(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }
} // namespace
