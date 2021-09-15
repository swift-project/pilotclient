/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorfsx.h"
#include "../fsxcommon/simconnectsymbols.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Weather;
using namespace BlackCore;

namespace BlackSimPlugin::Fsx
{
    CSimulatorFsx::CSimulatorFsx(const CSimulatorPluginInfo &info,
                                    IOwnAircraftProvider *ownAircraftProvider,
                                    IRemoteAircraftProvider *remoteAircraftProvider,
                                    IWeatherGridProvider *weatherGridProvider,
                                    IClientProvider *clientProvider,
                                    QObject *parent) :
        CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
    {
        this->setDefaultModel(
        {
            "Boeing 737-800 Paint1",
            CAircraftModel::TypeModelMatchingDefaultModel,
            "B737-800 default model",
            CAircraftIcaoCode("B738", "L2J")
        });
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
