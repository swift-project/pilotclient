// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/registermetadataaviation.h"

// Aviation headers
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/navsystem.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/aviation/track.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircraftcategory.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftvelocity.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/flightplanlist.h"
#include "blackmisc/aviation/simbriefdata.h"
#include "blackmisc/aviation/aircraftengine.h"
#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"

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
            CWakeTurbulenceCategory::registerMetadata();
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
