/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorp3dfactory.h"
#include "simulatorp3d.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QTimer>

namespace BlackSimPlugin
{
    namespace P3D
    {
        BlackCore::ISimulator *CSimulatorP3DFactory::create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                                            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                                            BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                                            BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider)
        {
            Q_ASSERT(ownAircraftProvider);
            return new CSimulatorP3D(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, this);
        }

        BlackCore::ISimulatorListener *CSimulatorP3DFactory::createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info)
        {
            return new CSimulatorP3DListener(info);
        }

    } // namespace
} // namespace
