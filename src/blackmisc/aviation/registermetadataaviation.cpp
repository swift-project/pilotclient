/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadataaviation.h"
#include "aviation.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Aviation
    {
        void registerMetadata()
        {
            CAdfSystem::registerMetadata();
            CAircraftEngine::registerMetadata();
            CAircraftEngineList::registerMetadata();
            CAircraftIcaoCode::registerMetadata();
            CAircraftIcaoCodeList::registerMetadata();
            CAircraftCategory::registerMetadata();
            CAircraftCategoryList::registerMetadata();
            CAircraftLights::registerMetadata();
            CAircraftParts::registerMetadata();
            CAircraftPartsList::registerMetadata();
            CAircraftSituation::registerMetadata();
            CAircraftSituationChange::registerMetadata();
            CAircraftSituationList::registerMetadata();
            CAirlineIcaoCode::registerMetadata();
            CAirlineIcaoCodeList::registerMetadata();
            CAirportIcaoCode::registerMetadata();
            CAirport::registerMetadata();
            CAirportList::registerMetadata();
            CAltitude::registerMetadata();
            CAtcStation::registerMetadata();
            CAtcStationList::registerMetadata();
            CCallsign::registerMetadata();
            CCallsignSet::registerMetadata();
            CComSystem::registerMetadata();
            CFlightPlan::registerMetadata();
            CFlightPlanList::registerMetadata();
            CSimBriefData::registerMetadata();
            CFlightPlanRemarks::registerMetadata();
            CHeading::registerMetadata();
            CInformationMessage::registerMetadata();
            CLivery::registerMetadata();
            CLiveryList::registerMetadata();
            CNavSystem::registerMetadata();
            CSelcal::registerMetadata();
            CTrack::registerMetadata();
            CTransponder::registerMetadata();
        }
    }

#if defined(Q_OS_WIN) && defined(Q_CC_CLANG)
    namespace Private
    {
        template void maybeRegisterMetaListConvert<Aviation::CAircraftEngineList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAircraftIcaoCodeList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAircraftCategoryList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAircraftPartsList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAircraftSituationList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAirlineIcaoCodeList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAirportList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CAtcStationList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CFlightPlanList>(int);
        template void maybeRegisterMetaListConvert<Aviation::CLiveryList>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAircraftEngine>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAircraftIcaoCode>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAircraftCategory>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAircraftParts>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAircraftSituation>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAirlineIcaoCode>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAirport>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CAtcStation>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CFlightPlan>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Aviation::CLivery>>(int);
    } // ns
#endif

} // ns
