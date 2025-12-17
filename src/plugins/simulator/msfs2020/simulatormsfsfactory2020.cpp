// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfsfactory2020.h"

#include "simulatormsfs2020.h"

#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core;

namespace swift::simplugin::msfs
{
    ISimulator *CSimulatorMsFsFactory::create(const CSimulatorPluginInfo &info,
                                              IOwnAircraftProvider *ownAircraftProvider,
                                              IRemoteAircraftProvider *remoteAircraftProvider,
                                              IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorMsFs(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    ISimulatorListener *CSimulatorMsFsFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorMsFsListener(info);
    }
} // namespace swift::simplugin::msfs
