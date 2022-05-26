/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/registermetadatasimulation.h"

// Simulation headers
#include "blackmisc/simulation/aircraftmatchersetup.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/matchinglog.h"
#include "blackmisc/simulation/reverselookup.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorinfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/fscommon/vpilotmodelruleset.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/swiftpluginsettings.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"

#include "blackmisc/variant.h"

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

            qRegisterMetaTypeStreamOperators<CSimulatorSettings::CGSource>();
            qRegisterMetaTypeStreamOperators<CInterpolationAndRenderingSetupBase::InterpolatorMode>();
            qRegisterMetaTypeStreamOperators<CAircraftMatcherSetup::MatchingAlgorithm>();
            qRegisterMetaTypeStreamOperators<CAircraftMatcherSetup::MatchingModeFlag>();
            qRegisterMetaTypeStreamOperators<MatchingLogFlag>();
            qRegisterMetaTypeStreamOperators<MatchingLog>();
            qRegisterMetaTypeStreamOperators<CAircraftMatcherSetup::PickSimilarStrategy>();
            qRegisterMetaTypeStreamOperators<ReverseLookupLoggingFlag>();
            qRegisterMetaTypeStreamOperators<ReverseLookupLogging>();
        }
    }

} // ns
