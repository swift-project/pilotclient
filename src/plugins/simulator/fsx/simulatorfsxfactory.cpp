// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfsxfactory.h"
#include "simulatorfsx.h"
#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core;

namespace BlackSimPlugin::Fsx
{
    ISimulator *CSimulatorFsxFactory::create(const CSimulatorPluginInfo &info,
                                             IOwnAircraftProvider *ownAircraftProvider,
                                             IRemoteAircraftProvider *remoteAircraftProvider,
                                             IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorFsx(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    ISimulatorListener *CSimulatorFsxFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorFsxListener(info);
    }
} // namespace
