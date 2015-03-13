/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blacksimfreefunctions.h"
#include "blacksim/simulatorinfolist.h"
#include "blacksim/setsimulator.h"
#include "fsx/simconnectutilities.h"
#include "fscommon/aircraftcfgentrieslist.h"

namespace BlackSim
{

    void registerMetadata()
    {
        BlackSim::CSimulatorPluginInfo::registerMetadata();
        BlackSim::CSimulatorPluginInfoList::registerMetadata();
        BlackSim::Settings::CSettingsSimulator::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntries::registerMetadata();
        BlackSim::FsCommon::CAircraftCfgEntriesList::registerMetadata();
        BlackSim::Fsx::CSimConnectUtilities::registerMetadata();
    }

} // namespace
