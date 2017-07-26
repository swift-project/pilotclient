/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/simulator.h"
#include "blackcore/application.h"

#include <QFlag>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    int ISimulator::getSimulatorStatus() const
    {
        int status =
            (isConnected() ? Connected : static_cast<ISimulator::SimulatorStatus>(0))
            | (isSimulating() ? Simulating : static_cast<ISimulator::SimulatorStatus>(0))
            | (isPaused() ? Paused : static_cast<ISimulator::SimulatorStatus>(0));
        return status;
    }

    QString ISimulator::statusToString(int status)
    {
        if (status > 0)
        {
            QString s;
            if (status & Connected) { s.append("Connected"); }
            if (status & Simulating) { if (!s.isEmpty()) { s.append(", "); } s.append("Simulating"); }
            if (status & Paused) { if (!s.isEmpty()) { s.append(", "); } s.append("Paused"); }
            return s;
        }
        else
        {
            return "Disconnected";
        }
    }

    ISimulator::SimulatorStatus ISimulator::statusToEnum(int status)
    {
        return static_cast<SimulatorStatus>(status);
    }

    ISimulator::ISimulator(QObject *parent) :
        QObject(parent),
        BlackMisc::CIdentifiable(this)
    {
        ISimulator::registerHelp();
    }

    void ISimulator::emitSimulatorCombinedStatus(SimulatorStatus oldStatus)
    {
        const SimulatorStatus newStatus = getSimulatorStatus();
        if (oldStatus != newStatus)
        {
            // decouple, follow up of signal can include unloading
            // simulator so this should happen asyncronously (which is like forcing Qt::QueuedConnection)
            QTimer::singleShot(0, this, [ = ]
            {
                emit this->simulatorStatusChanged(newStatus);
            });
        }
    }

    ISimulatorListener::ISimulatorListener(const CSimulatorPluginInfo &info) :
        QObject(), m_info(info)
    {
        this->setObjectName("ISimulatorListener:" + info.toQString());

        // stop listener after it reports simulator ready
        bool s = connect(this, &ISimulatorListener::simulatorStarted, this, &ISimulatorListener::stop, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "connect failed");
        Q_UNUSED(s)
    }

    QString ISimulatorListener::backendInfo() const
    {
        return m_info.toQString();
    }

    bool ISimulatorListener::isShuttingDown() const
    {
        return (!sApp || sApp->isShuttingDown());
    }

    void ISimulatorListener::start()
    {
        if (m_isRunning) { return; }
        m_isRunning = true;
        startImpl();
    }

    void ISimulatorListener::stop()
    {
        if (!m_isRunning) { return; }
        stopImpl();
        m_isRunning = false;
    }

} // namespace
