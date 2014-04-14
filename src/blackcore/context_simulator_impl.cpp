/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include <QPluginLoader>
#include "context_runtime.h"

#ifdef BLACK_WITH_FSX
#include "fsx/simulator_fsx.h"
#endif

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackCore
{
    CContextSimulator::CContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime),
        m_simulator(nullptr), m_updateTimer(nullptr)
    {
        m_updateTimer = new QTimer(this);
        loadPlugins();
        connect(m_updateTimer, &QTimer::timeout, this, &CContextSimulator::updateOwnAircraft);
    }

    CContextSimulator::~CContextSimulator() {}

    bool CContextSimulator::isConnected() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return false;
        return m_simulator->isConnected();
    }

    BlackMisc::Aviation::CAircraft CContextSimulator::getOwnAircraft() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return m_ownAircraft;
    }

    BlackSim::CSimulatorInfo CContextSimulator::getSimulatorInfo() const
    {
        if (this->getRuntime()->isSlotLogForSimulatorEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        if (!m_simulator) return BlackSim::CSimulatorInfo::UnspecifiedSim();
        return m_simulator->getSimulatorInfo();
    }

    void CContextSimulator::updateOwnAircraft()
    {
        m_ownAircraft = m_simulator->getOwnAircraft();
        getIContextNetwork()->updateOwnSituation(m_ownAircraft.getSituation());
        getIContextNetwork()->updateOwnCockpit(m_ownAircraft.getCom1System(), m_ownAircraft.getCom2System(), m_ownAircraft.getTransponder());
    }

    void CContextSimulator::setConnectionStatus(bool value)
    {
        if (value)
            m_updateTimer->start(100);
        else
            m_updateTimer->stop();
        emit connectionChanged(value);
    }

    void CContextSimulator::loadPlugins()
    {
        m_pluginsDir = QDir(qApp->applicationDirPath().append("/plugins"));
        if (!m_pluginsDir.exists())
        {
            qWarning() << "No plugin directory" << m_pluginsDir.currentPath();
            return;
        }

        foreach(QString fileName, m_pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(m_pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory *>(plugin);
                if (factory)
                {
                    m_simulator = factory->create(this);
                    connect(m_simulator, SIGNAL(connectionChanged(bool)), this, SLOT(setConnectionStatus(bool)));
                    qDebug() << "Simulator plugin:" << m_simulator->getSimulatorInfo().toQString();
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
