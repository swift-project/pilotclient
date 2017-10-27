/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorp3d.h"
#include "blackcore/application.h"
#include "blackmisc/threadutils.h"

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
    namespace P3D
    {
        CSimulatorP3D::CSimulatorP3D(const CSimulatorPluginInfo &info,
                                     IOwnAircraftProvider *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     IWeatherGridProvider *weatherGridProvider,
                                     QObject *parent) :
            CSimulatorFsxCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, parent)
        {
            m_defaultModel =
            {
                "LOCKHEED L049_2",
                CAircraftModel::TypeModelMatchingDefaultModel,
                "Constellation in TWA livery",
                CAircraftIcaoCode("CONI", "L4P")
            };
        }
    } // namespace
} // namespace
