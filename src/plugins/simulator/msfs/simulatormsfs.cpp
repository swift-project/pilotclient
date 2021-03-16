/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatormsfs.h"
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

namespace BlackSimPlugin
{
    namespace Msfs
    {
        CSimulatorMsFs::CSimulatorMsFs(const CSimulatorPluginInfo &info,
                                     IOwnAircraftProvider *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     IWeatherGridProvider *weatherGridProvider,
                                     IClientProvider *clientProvider,
                                     QObject *parent) :
            CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            this->setDefaultModel(
            {
                "Airbus A320 Neo Asobo",
                CAircraftModel::TypeModelMatchingDefaultModel,
                "Airbus A320 default model",
                CAircraftIcaoCode("A320", "L2J")
            });
        }

        bool CSimulatorMsFs::connectTo()
        {
        #ifdef Q_OS_WIN64
            if (!loadAndResolveMSFSimConnect()) { return false; }
            return CSimulatorFsxCommon::connectTo();
        #else
            if (!loadAndResolveFsxSimConnect(true)) { return false; }
            return CSimulatorFsxCommon::connectTo();
        #endif
        }

        void CSimulatorMsFsListener::startImpl()
        {
        #ifdef Q_OS_WIN64
            if (!loadAndResolveMSFSimConnect()) { return; }
            CSimulatorFsxCommonListener::startImpl();
        #else
            if (!loadAndResolveFsxSimConnect(true)) { return; }
            CSimulatorFsxCommonListener::startImpl();
        #endif
        }

    } // ns
} // ns
