/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator.h"
#include "interpolator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/collection.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    const QString &ISimulator::simulatorOriginator()
    {
        // string is generated once, the timestamp allows to use multiple
        // components (as long as they are not generated at the same ms)
        static const QString o = QString("SIMULATOR:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
        return o;
    }

    QString ISimulator::statusToString(int status)
    {
        if (status > 0)
        {
            QString s;
            if (status & Connected) { s.append("Connected"); }
            if (status & Running) { if (!s.isEmpty()) { s.append(", "); } s.append("Simulating"); }
            if (status & Paused) { if (!s.isEmpty()) { s.append(", "); } s.append("Paused"); }
            return s;
        }
        else
        {
            return "Disconnected";
        }
    }

    void ISimulator::emitSimulatorCombinedStatus()
    {
        int status =
            (isConnected() ? Connected : static_cast<ISimulator::SimulatorStatus>(0))
            | (isSimulating() ? Running : static_cast<ISimulator::SimulatorStatus>(0))
            | (isPaused() ? Paused : static_cast<ISimulator::SimulatorStatus>(0));
        emit simulatorStatusChanged(status);
    }

    ISimulatorListener::ISimulatorListener(QObject *parent) : QObject(parent)
    { }

} // namespace
