// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
