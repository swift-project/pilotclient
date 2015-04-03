/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorinfolist.h"
#include "blackmisc/simulation/setsimulator.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Settings;


/*
 * Metadata for simulation
 *
 * In a separate file to workaround a limitation of MinGW:
 * http://stackoverflow.com/q/16596876/1639256
 */
void BlackMisc::Simulation::registerMetadata()
{
    CSimulatorPluginInfo::registerMetadata();
    CSimulatorPluginInfoList::registerMetadata();
    CSettingsSimulator::registerMetadata();
    FsCommon::CAircraftCfgEntries::registerMetadata();
    FsCommon::CAircraftCfgEntriesList::registerMetadata();
    Fsx::CSimConnectUtilities::registerMetadata();
    CAircraftModel::registerMetadata();
    CAircraftModelList::registerMetadata();
    CSimulatedAircraft::registerMetadata();
    CSimulatedAircraftList::registerMetadata();
}
