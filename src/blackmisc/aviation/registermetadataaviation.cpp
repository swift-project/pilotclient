/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/registermetadataaviation.h"
#include "blackmisc/aviation/aviation.h"

namespace BlackMisc
{
    namespace Aviation
    {
        void registerMetadata()
        {
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
            CAircraftVelocity::registerMetadata();
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

} // ns
