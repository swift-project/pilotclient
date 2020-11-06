/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadatasimulation.h"
#include "simulation.h"
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

#if defined(Q_OS_WIN) && defined(Q_CC_CLANG)
    namespace Private
    {
        template void maybeRegisterMetaListConvert<Simulation::FsCommon::CAircraftCfgEntriesList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CAircraftModelList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CDistributorList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CInterpolationSetupList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CMatchingStatistics>(int);
        template void maybeRegisterMetaListConvert<Simulation::CSimulatedAircraftList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CSimulatorInfoList>(int);
        template void maybeRegisterMetaListConvert<Simulation::CSimulatorPluginInfoList>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::FsCommon::CAircraftCfgEntries>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CAircraftModel>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CDistributor>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CInterpolationAndRenderingSetupPerCallsign>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CMatchingStatisticsEntry>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CSimulatedAircraft>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CSimulatorInfo>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Simulation::CSimulatorPluginInfo>>(int);
    } // ns
#endif

} // ns
