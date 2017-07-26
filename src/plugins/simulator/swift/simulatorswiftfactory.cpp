/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorswiftfactory.h"
#include "simulatorswift.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QTimer>

namespace BlackSimPlugin
{
    namespace Swift
    {
        BlackCore::ISimulator *CSimulatorSwiftFactory::create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider)
        {
            Q_ASSERT(ownAircraftProvider);
            return new CSimulatorSwift(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, this);
        }

        BlackCore::ISimulatorListener *CSimulatorSwiftFactory::createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info)
        {
            return new CSimulatorSwiftListener(info);
        }
    } // namespace
} // namespace
