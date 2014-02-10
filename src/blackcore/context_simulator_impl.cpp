/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_simulator_impl.h"
#include "coreruntime.h"
#include "fsx/simulator_fsx.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackCore
{
    // Init this context
    CContextSimulator::CContextSimulator(QObject *parent) :
        IContextSimulator(parent),
        m_simulator(new BlackCore::FSX::CSimulatorFSX(this)),
        m_updateTimer(nullptr),
        m_contextNetwork(nullptr)

    {
        m_updateTimer = new QTimer(this);
        connect(m_simulator, &ISimulator::connectionChanged, this, &CContextSimulator::setConnectionStatus);
        connect(m_updateTimer, &QTimer::timeout, this, &CContextSimulator::updateOwnAircraft);
    }

    // Cleanup
    CContextSimulator::~CContextSimulator()
    {
    }

    bool CContextSimulator::isConnected() const
    {
        return m_simulator->isConnected();
    }

    BlackMisc::Aviation::CAircraft CContextSimulator::ownAircraft() const
    {
        return m_ownAircraft;
    }

    void CContextSimulator::updateOwnAircraft()
    {
        m_ownAircraft = m_simulator->getOwnAircraft();

        if (!m_contextNetwork)
        {
            m_contextNetwork = getRuntime()->getIContextNetwork();
        }

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

} // namespace BlackCore
