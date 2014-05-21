/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include "context_ownaircraft_impl.h"
#include "context_runtime.h"

#include <QPluginLoader>
#include <QLibrary>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
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

    void CContextSimulator::updateOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
        this->getRuntime()->getCContextOwnAircraft()->updateOwnAircraft(this->m_simulator->getOwnAircraft(), IContextSimulator::InterfaceName());
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
        this->m_canConnectResult = QtConcurrent::run(this, &CContextSimulator::connectTo);
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
        return this->m_aircraftModel;
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

        connect(m_simulator, SIGNAL(connectionChanged(bool)), this, SLOT(setConnectionStatus(bool)));
        return true;
    }

    void CContextSimulator::unloadSimulatorPlugin()
    {
        if (m_simulator)
            m_simulator->deleteLater();

        m_simulator = nullptr;
    }

    void CContextSimulator::setConnectionStatus(bool value)
    {
        if (value)
            m_updateTimer->start(100);
        else
            m_updateTimer->stop();
        emit connectionChanged(value);
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

            QPluginLoader loader(m_pluginsDir.absoluteFilePath(fileName));
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
            }
        }
    }

} // namespace BlackCore
