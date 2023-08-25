// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorp3dfactory.h"
#include "simulatorp3d.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Network;

namespace BlackSimPlugin::P3D
{
    BlackCore::ISimulator *CSimulatorP3DFactory::create(const CSimulatorPluginInfo &info,
                                                        IOwnAircraftProvider *ownAircraftProvider,
                                                        IRemoteAircraftProvider *remoteAircraftProvider,
                                                        IWeatherGridProvider *weatherGridProvider,
                                                        IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorP3D(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
    }

    BlackCore::ISimulatorListener *CSimulatorP3DFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorP3DListener(info);
    }
} // namespace
