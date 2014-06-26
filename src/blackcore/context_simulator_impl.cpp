/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include "context_ownaircraft.h"
#include <QPluginLoader>
#include <QLibrary>
#include "context_runtime.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackSim;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime),
        m_simulator(nullptr), m_updateTimer(nullptr)
    {
        m_updateTimer = new QTimer(this);
        findSimulatorPlugins();
        loadSimulatorPlugin(CSimulatorInfo::FSX());

        connect(m_updateTimer, &QTimer::timeout, this, &CContextSimulator::updateOwnAircraft);
        asyncConnectTo();
    }

    CContextSimulator::~CContextSimulator()
    {
        disconnectFrom();
        unloadSimulatorPlugin();
    }

    CSimulatorInfoList CContextSimulator::getAvailableSimulatorPlugins() const
    {
        CSimulatorInfoList simulatorPlugins;
        foreach(ISimulatorFactory * factory, m_simulatorFactories)
        {
            simulatorPlugins.push_back(factory->getSimulatorInfo());
        }
        return simulatorPlugins;
    }

    bool CContextSimulator::isConnected() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->isConnected();
    }

    bool CContextSimulator::canConnect()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->canConnect();
    }

    bool CContextSimulator::connectTo()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->connectTo();
    }

    void CContextSimulator::asyncConnectTo()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator || m_canConnectResult.isRunning()) return; // already checking
        m_simulator->asyncConnectTo();
    }

    bool CContextSimulator::disconnectFrom()
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->disconnectFrom();
    }

    BlackSim::CSimulatorInfo CContextSimulator::getSimulatorInfo() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return BlackSim::CSimulatorInfo::UnspecifiedSim();
        return m_simulator->getSimulatorInfo();
    }

    Network::CAircraftModel CContextSimulator::getOwnAircraftModel() const
    {
        // If no ISimulator object is available, return a dummy.
        if (!m_simulator)
            return Network::CAircraftModel();

        return this->m_simulator->getAircraftModel();
    }

    bool CContextSimulator::loadSimulatorPlugin(const CSimulatorInfo &simulatorInfo)
    {
        ISimulatorFactory *factory = nullptr;
        QSet<ISimulatorFactory *>::iterator iterator = std::find_if(m_simulatorFactories.begin(), m_simulatorFactories.end(), [ = ](const ISimulatorFactory * factory)
        {
            return factory->getSimulatorInfo() == simulatorInfo;
        });

        if (iterator == m_simulatorFactories.end())
            return false;

        factory = *iterator;
        Q_ASSERT(factory);

        m_simulator = factory->create(this);
        Q_ASSERT(m_simulator);

        connect(m_simulator, SIGNAL(statusChanged(ISimulator::Status)), this, SLOT(setConnectionStatus(ISimulator::Status)));
        connect(m_simulator, &ISimulator::aircraftModelChanged, this, &IContextSimulator::ownAircraftModelChanged);
        return true;
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (m_simulator)
            m_simulator->deleteLater();

        m_simulator = nullptr;
    }

    void CContextSimulator::updateOwnAircraft()
    {
        Q_ASSERT(this->getIContextOwnAircraft());

        // we make sure not to override values we do not have
        CAircraft aircraft = this->getIContextOwnAircraft()->getOwnAircraft();
        CAircraft simulatorAircraft = this->m_simulator->getOwnAircraft();
        aircraft.setSituation(simulatorAircraft.getSituation());
        aircraft.setCockpit(simulatorAircraft.getCom1System(), simulatorAircraft.getCom2System(), simulatorAircraft.getTransponderCode());

        Q_ASSERT(this->getIContextOwnAircraft()); // paranoia against context having been deleted from another thread - redmine issue #270
        if (this->getIContextOwnAircraft())
        {
            // the method will check, if an update is really required
            // these are local (non DBus) calls
            this->getIContextOwnAircraft()->updateOwnAircraft(aircraft, this->getPathAndContextId());
        }
    }

    void CContextSimulator::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &initialSituation)
    {
        Q_ASSERT(this->m_simulator);
        this->m_simulator->addAircraftSituation(callsign, initialSituation);
    }

    void CContextSimulator::updateCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        Q_ASSERT(this->m_simulator);

        // avoid loops
        if (originator.isEmpty() || originator == IContextSimulator::InterfaceName()) return;

        // update
        this->m_simulator->updateOwnSimulatorCockpit(ownAircraft);
    }

    void CContextSimulator::setConnectionStatus(ISimulator::Status status)
    {
        if (status == ISimulator::Connected)
        {
            m_updateTimer->start(100);
            emit connectionChanged(true);
        }
        else
        {
            m_updateTimer->stop();
            emit connectionChanged(false);
        }
    }

    void CContextSimulator::statusMessageReceived(const CStatusMessage &statusMessage)
    {
        if (statusMessage.getSeverity() != CStatusMessage::SeverityError) return;
        this->m_simulator->displayStatusMessage(statusMessage);
    }

    void CContextSimulator::statusMessagesReceived(const CStatusMessageList &statusMessages)
    {
        foreach(CStatusMessage m, statusMessages)
        {
            this->statusMessageReceived(m);
        }
    }

    void CContextSimulator::textMessagesReceived(const Network::CTextMessageList &textMessages)
    {
        foreach(CTextMessage tm, textMessages)
        {
            if (!tm.isPrivateMessage()) continue;
            this->m_simulator->displayStatusMessage(tm.asStatusMessage(true, true));
        }
    }

    void CContextSimulator::findSimulatorPlugins()
    {
        m_pluginsDir = QDir(qApp->applicationDirPath().append("/plugins/simulator"));
        if (!m_pluginsDir.exists())
        {
            qWarning() << "No plugin directory" << m_pluginsDir.currentPath();
            return;
        }

        foreach(QString fileName, m_pluginsDir.entryList(QDir::Files))
        {
            if (!QLibrary::isLibrary(fileName))
                continue;

            QString pluginPath = m_pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginPath);
            QObject *plugin = loader.instance();
            if (plugin)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory *>(plugin);
                if (factory)
                {
                    CSimulatorInfo simulatorInfo = factory->getSimulatorInfo();
                    qDebug() << "Found simulator plugin: " << simulatorInfo.toQString();
                    m_simulatorFactories.insert(factory);
                    break;
                }
            }
            else
            {
                qDebug() << loader.errorString();
                qDebug() << "Also check if required dll/libs of plugin exists";
            }
        }
    }

} // namespace
