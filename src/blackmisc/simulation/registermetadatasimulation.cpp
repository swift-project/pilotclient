/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadatasimulation.h"
#include "simulation.h"

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Settings;

namespace BlackMisc
{
    namespace Simulation
    {
        void registerMetadata()
        {
            CAircraftCfgEntries::registerMetadata();
            CAircraftCfgEntriesList::registerMetadata();
            CAircraftModel::registerMetadata();
            CAircraftModelList::registerMetadata();
            CAirspaceAircraftSnapshot::registerMetadata();
            CDistributor::registerMetadata();
            CDistributorList::registerMetadata();
            CDistributorListPreferences::registerMetadata();
            qDBusRegisterMetaType<CInterpolationAndRenderingSetupBase::InterpolatorMode>();
            CInterpolationAndRenderingSetupPerCallsign::registerMetadata();
            CInterpolationAndRenderingSetupGlobal::registerMetadata();
            CInterpolationSetupList::registerMetadata();
            CMatchingStatisticsEntry::registerMetadata();
            CMatchingStatistics::registerMetadata();
            CModelSettings::registerMetadata();
            CSimConnectUtilities::registerMetadata();
            CSimulatedAircraft::registerMetadata();
            CSimulatedAircraftList::registerMetadata();
            CSimulatorInfo::registerMetadata();
            CSimulatorInfoList::registerMetadata();
            CSimulatorInternals::registerMetadata();
            CSimulatorMessagesSettings::registerMetadata();
            CSimulatorPluginInfo::registerMetadata();
            CSimulatorPluginInfoList::registerMetadata();
            CSimulatorSettings::registerMetadata();
            CSwiftPluginSettings::registerMetadata();
            CVPilotModelRule::registerMetadata();
            CVPilotModelRuleSet::registerMetadata();
            qRegisterMetaType<CAircraftMatcherSetup::MatchingAlgorithm>();
            qRegisterMetaType<CAircraftMatcherSetup::MatchingMode>();
            qRegisterMetaType<CAircraftMatcherSetup::MatchingModeFlag>();
            qDBusRegisterMetaType<CAircraftMatcherSetup::MatchingAlgorithm>();
            qDBusRegisterMetaType<CAircraftMatcherSetup::MatchingModeFlag>();
            CAircraftMatcherSetup::registerMetadata();
        }
    } // ns
} // ns
