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
    ISimulator::SimulatorStatus ISimulator::getSimulatorStatus() const
    {
        if (!this->isConnected()) { return Disconnected; }
        const SimulatorStatus status =
            Connected
            | (this->isSimulating() ? Simulating : static_cast<ISimulator::SimulatorStatusFlag>(0))
            | (this->isPaused() ? Paused : static_cast<ISimulator::SimulatorStatusFlag>(0));
        return status;
    }

    CSimulatorInfo ISimulator::getSimulatorInfo() const
    {
        return CSimulatorInfo(this->getSimulatorPluginInfo().getSimulatorInfo());
    }

    void ISimulator::registerHelp()
    {
        if (BlackMisc::CSimpleCommandParser::registered("BlackCore::ISimulator")) { return; }
        BlackMisc::CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
        BlackMisc::CSimpleCommandParser::registerCommand({".drv unload", "unload driver"});
        if (BlackConfig::CBuildConfig::isCompiledWithFsuipcSupport())
        {
            BlackMisc::CSimpleCommandParser::registerCommand({".drv fsuipc on|off", "enable/disable FSUIPC (if applicable)"});
        }
    }

    QString ISimulator::statusToString(SimulatorStatus status)
    {
        QStringList s;
        if (status.testFlag(Unspecified)) s << "Unspecified";
        if (status.testFlag(Disconnected)) s << "Disconnected";
        if (status.testFlag(Connected)) s << "Connected";
        if (status.testFlag(Simulating)) s << "Simulating";
        if (status.testFlag(Paused)) s << "Paused";
        return s.join(", ");
    }

    ISimulator::ISimulator(QObject *parent) :
        QObject(parent),
        BlackMisc::CIdentifiable(this)
    {
        ISimulator::registerHelp();
    }

    void ISimulator::emitSimulatorCombinedStatus(SimulatorStatus oldStatus)
    {
        const SimulatorStatus newStatus = this->getSimulatorStatus();
        if (oldStatus != newStatus)
        {
            // decouple, follow up of signal can include unloading
            // simulator so this should happen strictly asyncronously (which is like forcing Qt::QueuedConnection)
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
        const bool s = connect(this, &ISimulatorListener::simulatorStarted, this, &ISimulatorListener::stop, Qt::QueuedConnection);
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
        this->startImpl();
    }

    void ISimulatorListener::stop()
    {
        if (!m_isRunning) { return; }
        this->stopImpl();
        m_isRunning = false;
    }

} // namespace
