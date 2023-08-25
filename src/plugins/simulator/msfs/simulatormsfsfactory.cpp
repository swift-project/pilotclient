// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfsfactory.h"
#include "simulatormsfs.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;

namespace BlackSimPlugin::Msfs
{
    ISimulator *CSimulatorMsFsFactory::create(const CSimulatorPluginInfo &info,
                                              IOwnAircraftProvider *ownAircraftProvider,
                                              IRemoteAircraftProvider *remoteAircraftProvider,
                                              IWeatherGridProvider *weatherGridProvider,
                                              IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorMsFs(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
    }

    ISimulatorListener *CSimulatorMsFsFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorMsFsListener(info);
    }
} // namespace
