// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatoremulatedfactory.h"
#include "simulatoremulated.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QTimer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin::Emulated
{
    BlackCore::ISimulator *CSimulatorEmulatedFactory::create(const CSimulatorPluginInfo &info,
                                                             IOwnAircraftProvider *ownAircraftProvider,
                                                             IRemoteAircraftProvider *remoteAircraftProvider,
                                                             BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider, Network::IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorEmulated(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
    }

    BlackCore::ISimulatorListener *CSimulatorEmulatedFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorEmulatedListener(info);
    }
} // namespace
