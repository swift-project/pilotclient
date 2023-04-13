/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorfsxfactory.h"
#include "simulatorfsx.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;

namespace BlackSimPlugin::Fsx
{
    ISimulator *CSimulatorFsxFactory::create(const CSimulatorPluginInfo &info,
                                             IOwnAircraftProvider *ownAircraftProvider,
                                             IRemoteAircraftProvider *remoteAircraftProvider,
                                             IWeatherGridProvider *weatherGridProvider,
                                             IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorFsx(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
    }

    ISimulatorListener *CSimulatorFsxFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorFsxListener(info);
    }
} // namespace
