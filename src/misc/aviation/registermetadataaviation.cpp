// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/registermetadataaviation.h"

// Aviation headers
#include "misc/aviation/aircraftcategory.h"
#include "misc/aviation/aircraftcategorylist.h"
#include "misc/aviation/aircraftengine.h"
#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/aircraftlights.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/aircraftsituationchange.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/aircraftvelocity.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/airport.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/flightplan.h"
#include "misc/aviation/flightplanlist.h"
#include "misc/aviation/heading.h"
#include "misc/aviation/informationmessage.h"
#include "misc/aviation/livery.h"
#include "misc/aviation/liverylist.h"
#include "misc/aviation/navsystem.h"
#include "misc/aviation/ongroundinfo.h"
#include "misc/aviation/selcal.h"
#include "misc/aviation/simbriefdata.h"
#include "misc/aviation/track.h"
#include "misc/aviation/transponder.h"

namespace swift::misc
{
    namespace aviation
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
            CSsrEquipment::registerMetadata();
            CComNavEquipment::registerMetadata();
            CWakeTurbulenceCategory::registerMetadata();
            CFlightPlan::registerMetadata();
            CFlightPlanAircraftInfo::registerMetadata();
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
            COnGroundInfo::registerMetadata();
        }
    } // namespace aviation

} // namespace swift::misc
