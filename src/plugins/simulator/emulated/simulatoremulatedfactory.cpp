// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatoremulatedfactory.h"

#include <QTimer>

#include "simulatoremulated.h"

#include "misc/simulation/simulatorplugininfo.h"

using namespace swift::misc;
using namespace swift::misc::simulation;

namespace swift::simplugin::emulated
{
    swift::core::ISimulator *CSimulatorEmulatedFactory::create(const CSimulatorPluginInfo &info,
                                                               IOwnAircraftProvider *ownAircraftProvider,
                                                               IRemoteAircraftProvider *remoteAircraftProvider,
                                                               network::IClientProvider *clientProvider)
    {
        Q_ASSERT(ownAircraftProvider);
        return new CSimulatorEmulated(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    swift::core::ISimulatorListener *CSimulatorEmulatedFactory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorEmulatedListener(info);
    }
} // namespace swift::simplugin::emulated
