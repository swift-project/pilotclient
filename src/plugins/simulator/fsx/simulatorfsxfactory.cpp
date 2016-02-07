/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfsxfactory.h"
#include "simulatorfsx.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/project.h"
#include <QTimer>

namespace BlackSimPlugin
{
    namespace Fsx
    {
        BlackCore::ISimulator *CSimulatorFsxFactory::create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                                            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                                            BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                                            BlackMisc::IPluginStorageProvider *pluginStorageProvider)
        {
            Q_ASSERT(ownAircraftProvider);
            return new CSimulatorFsx(info, ownAircraftProvider, remoteAircraftProvider, pluginStorageProvider, this);
        }

        BlackCore::ISimulatorListener *CSimulatorFsxFactory::createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info)
        {
            return new CSimulatorFsxListener(info);
        }

    } // namespace
} // namespace
