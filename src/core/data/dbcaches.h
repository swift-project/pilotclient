// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_DBCACHES
#define SWIFT_CORE_DATA_DBCACHES

#include "misc/aviation/aircraftcategorylist.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/liverylist.h"
#include "misc/countrylist.h"
#include "misc/datacache.h"
#include "misc/network/url.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributorlist.h"

namespace swift::core::data
{
    //! Trait for DB model cache
    struct TDbModelCache : public swift::misc::TDataTrait<swift::misc::simulation::CAircraftModelList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbmodelcache"; }
    };

    //! Trait for DB distributor cache
    struct TDbDistributorCache : public swift::misc::TDataTrait<swift::misc::simulation::CDistributorList>
    {
        //! First load is synchronous, distributors is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbdistributorcache"; }
    };

    //! Trait for DB liveries
    struct TDbLiveryCache : public swift::misc::TDataTrait<swift::misc::aviation::CLiveryList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbliverycache"; }
    };

    //! Trait for DB airline ICAO codes
    struct TDbAirlineIcaoCache : public swift::misc::TDataTrait<swift::misc::aviation::CAirlineIcaoCodeList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbairlineicaocache"; }
    };

    //! Trait for DB aircraft ICAO codes
    struct TDbAircraftIcaoCache : public swift::misc::TDataTrait<swift::misc::aviation::CAircraftIcaoCodeList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbaircrafticaocache"; }
    };

    //! Trait for DB countries
    struct TDbCountryCache : public swift::misc::TDataTrait<swift::misc::CCountryList>
    {
        //! First load is synchronous, countries is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbcountrycache"; }
    };

    //! Trait for DB categories
    struct TDbAircraftCategoryCache : public swift::misc::TDataTrait<swift::misc::aviation::CAircraftCategoryList>
    {
        //! First load is synchronous, countries is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbaircraftcategorycache"; }
    };

    //! Trait for airport list
    struct TDbAirportCache : public swift::misc::TDataTrait<swift::misc::aviation::CAirportList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbairportcache"; }
    };

    //! Trait for ICAO reader base URL
    struct TDbIcaoReaderBaseUrl : public swift::misc::TDataTrait<swift::misc::network::CUrl>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbicaoreaderurl"; }
    };

    //! Trait for ICAO reader base URL
    struct TDbModelReaderBaseUrl : public swift::misc::TDataTrait<swift::misc::network::CUrl>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbmodelreaderurl"; }
    };
} // namespace swift::core::data

#endif // SWIFT_CORE_DATA_DBCACHES
