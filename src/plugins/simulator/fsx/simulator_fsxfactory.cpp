/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator_fsxfactory.h"
#include "simulator_fsx.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/project.h"
#include <QTimer>
#include <QtConcurrent>

using namespace BlackSim;
using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace Fsx
    {
        BlackCore::ISimulator *CSimulatorFsxFactory::create(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider, BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider, QObject *parent)
        {
            Q_ASSERT(ownAircraftProvider);
            return new CSimulatorFsx(ownAircraftProvider, renderedAircraftProvider, parent);
        }

        CSimulatorInfo CSimulatorFsxFactory::getSimulatorInfo() const
        {
            return CSimulatorInfo::FSX();
        }

    } // namespace
} // namespace
