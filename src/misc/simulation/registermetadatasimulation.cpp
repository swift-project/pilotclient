// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/registermetadatasimulation.h"

// Simulation headers
#include "misc/simulation/aircraftmatchersetup.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/distributorlist.h"
#include "misc/simulation/distributorlistpreferences.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "misc/simulation/matchingstatistics.h"
#include "misc/simulation/matchinglog.h"
#include "misc/simulation/reverselookup.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/simulation/simulatorinfolist.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/simulation/simulatorplugininfolist.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/fscommon/vpilotmodelruleset.h"
#include "misc/simulation/fsx/simconnectutilities.h"
#include "misc/simulation/settings/fgswiftbussettings.h"
#include "misc/simulation/settings/modelsettings.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/settings/swiftpluginsettings.h"
#include "misc/simulation/settings/xswiftbussettings.h"

#include "misc/variant.h"

using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fsx;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::settings;

namespace swift::misc::simulation
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
        CFsxP3DSettings::registerMetadata();
        CSwiftPluginSettings::registerMetadata();
        CVPilotModelRule::registerMetadata();
        CVPilotModelRuleSet::registerMetadata();
        CAircraftMatcherSetup::registerMetadata();
        CXSwiftBusSettings::registerMetadata();
        CFGSwiftBusSettings::registerMetadata();

        qRegisterMetaType<CSimulatorSettings::CGSource>();
        qRegisterMetaType<CAircraftMatcherSetup::MatchingAlgorithm>();
        qRegisterMetaType<CAircraftMatcherSetup::MatchingMode>();
        qRegisterMetaType<CAircraftMatcherSetup::MatchingModeFlag>();
        qRegisterMetaType<CAircraftMatcherSetup::PickSimilarStrategy>();
        qRegisterMetaType<MatchingLog>();
        qRegisterMetaType<MatchingLogFlag>();
        qRegisterMetaType<ReverseLookupLoggingFlag>();
        qRegisterMetaType<ReverseLookupLogging>();

        // the ones generate with Q_DECLARE_FLAGS and no streamin operator fail here
        qDBusRegisterMetaType<CSimulatorSettings::CGSource>();
        qDBusRegisterMetaType<CInterpolationAndRenderingSetupBase::InterpolatorMode>();
        qDBusRegisterMetaType<CAircraftMatcherSetup::MatchingAlgorithm>();
        qDBusRegisterMetaType<CAircraftMatcherSetup::MatchingModeFlag>();
        qDBusRegisterMetaType<CAircraftMatcherSetup::PickSimilarStrategy>();
        qDBusRegisterMetaType<MatchingLogFlag>();
        qDBusRegisterMetaType<MatchingLog>();
        qDBusRegisterMetaType<ReverseLookupLoggingFlag>();
        qDBusRegisterMetaType<ReverseLookupLogging>();
    }

} // ns
