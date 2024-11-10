// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorp3dfactory.h"
#include "simulatorp3d.h"
#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::misc::simulation;
using namespace swift::misc::network;

namespace BlackSimPlugin::P3D
{
    swift::core::ISimulator *CSimulatorP3DFactory::create(const CSimulatorPluginInfo &info,
                                                          IOwnAircraftProvider *ownAircraftProvider,
                                                          IRemoteAircraftProvider *remoteAircraftProvider,
                                                          IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorP3D(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    swift::core::ISimulatorListener *CSimulatorP3DFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorP3DListener(info);
    }
} // namespace
