// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfsx.h"
#include "../fsxcommon/simconnectsymbols.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackCore;

namespace BlackSimPlugin::Fsx
{
    CSimulatorFsx::CSimulatorFsx(const CSimulatorPluginInfo &info,
                                 IOwnAircraftProvider *ownAircraftProvider,
                                 IRemoteAircraftProvider *remoteAircraftProvider,
                                 IClientProvider *clientProvider,
                                 QObject *parent) : CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent)
    {
        this->setDefaultModel(
            { "Boeing 737-800 Paint1",
              CAircraftModel::TypeModelMatchingDefaultModel,
              "B737-800 default model",
              CAircraftIcaoCode("B738", "L2J") });
    }

    bool CSimulatorFsx::connectTo()
    {
        if (!loadAndResolveFsxSimConnect(true)) { return false; }
        return CSimulatorFsxCommon::connectTo();
    }

    void CSimulatorFsxListener::startImpl()
    {
        if (!loadAndResolveFsxSimConnect(true)) { return; }
        return CSimulatorFsxCommonListener::startImpl();
    }

} // ns
