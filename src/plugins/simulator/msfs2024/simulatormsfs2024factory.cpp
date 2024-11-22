// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfs2024factory.h"

#include "simulatormsfs2024.h"

#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core;

namespace swift::simplugin::msfs2024
{
    ISimulator *CSimulatorMsFs2024Factory::create(const CSimulatorPluginInfo &info,
                                                  IOwnAircraftProvider *ownAircraftProvider,
                                                  IRemoteAircraftProvider *remoteAircraftProvider,
                                                  IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorMsFs2024(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    ISimulatorListener *CSimulatorMsFs2024Factory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorMsFs2024Listener(info);
    }
} // namespace swift::simplugin::msfs2024
