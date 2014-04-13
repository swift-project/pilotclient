/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include "coreruntime.h"
#include <QPluginLoader>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackCore
{
    // Init this context
    CContextSimulator::CContextSimulator(QObject *parent) :
        IContextSimulator(parent),
        m_simulator(nullptr),
        m_updateTimer(nullptr),
        m_contextNetwork(nullptr)
    {
        m_updateTimer = new QTimer(this);

        connect(m_updateTimer, &QTimer::timeout, this, &CContextSimulator::updateOwnAircraft);
    }

    // Cleanup
    CContextSimulator::~CContextSimulator()
    {
    }

    bool CContextSimulator::isConnected() const
    {
        if (!m_simulator)
            return false;

        return m_simulator->isConnected();
    }

    BlackMisc::Aviation::CAircraft CContextSimulator::getOwnAircraft() const
    {
        return m_ownAircraft;
    }

    void CContextSimulator::init()
    {
        loadPlugins();

        if (!m_contextNetwork)
        {
            m_contextNetwork = getRuntime()->getIContextNetwork();
        }

        if (m_simulator)
            connect(m_contextNetwork, SIGNAL(aircraftSituationUpdate(BlackMisc::Aviation::CCallsign,BlackMisc::Aviation::CAircraftSituation)),
                    m_simulator, SLOT(addAircraftSituation(BlackMisc::Aviation::CCallsign,BlackMisc::Aviation::CAircraftSituation)));
    }

    void CContextSimulator::updateOwnAircraft()
    {
        if (!m_simulator)
            return;

        m_ownAircraft = m_simulator->getOwnAircraft();



        m_contextNetwork->updateOwnSituation(m_ownAircraft.getSituation());
        m_contextNetwork->updateOwnCockpit(m_ownAircraft.getCom1System(), m_ownAircraft.getCom2System(), m_ownAircraft.getTransponder());
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
        m_pluginsDir = QDir(qApp->applicationDirPath());
        m_pluginsDir.cd("plugins");

        foreach (QString fileName, m_pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(m_pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin)
            {
                ISimulatorFactory *factory = qobject_cast<ISimulatorFactory*>(plugin);
                if(factory)
                {
                    m_simulator = factory->create(this);
                    connect(m_simulator, SIGNAL(connectionChanged(bool)), this, SLOT(setConnectionStatus(bool)));
                }

            }
            else
            {
                qDebug() << loader.errorString();
            }
        }
    }

} // namespace BlackCore
