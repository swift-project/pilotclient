/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_DBCACHES
#define BLACKCORE_DATA_DBCACHES

#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/countrylist.h"

#include <QDateTime>
#include <QObject>

namespace BlackCore::Data
{
    //! Trait for DB model cache
    struct TDbModelCache : public BlackMisc::TDataTrait<BlackMisc::Simulation::CAircraftModelList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbmodelcache"; }
    };

    //! Trait for DB distributor cache
    struct TDbDistributorCache : public BlackMisc::TDataTrait<BlackMisc::Simulation::CDistributorList>
    {
        //! First load is synchronous, distributors is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbdistributorcache"; }
    };

    //! Trait for DB liveries
    struct TDbLiveryCache : public BlackMisc::TDataTrait<BlackMisc::Aviation::CLiveryList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbliverycache"; }
    };

    //! Trait for DB airline ICAO codes
    struct TDbAirlineIcaoCache : public BlackMisc::TDataTrait<BlackMisc::Aviation::CAirlineIcaoCodeList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbairlineicaocache"; }
    };


    //! Trait for DB aircraft ICAO codes
    struct TDbAircraftIcaoCache : public BlackMisc::TDataTrait<BlackMisc::Aviation::CAircraftIcaoCodeList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbaircrafticaocache"; }
    };

    //! Trait for DB countries
    struct TDbCountryCache : public BlackMisc::TDataTrait<BlackMisc::CCountryList>
    {
        //! First load is synchronous, countries is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbcountrycache"; }
    };

    //! Trait for DB categories
    struct TDbAircraftCategoryCache : public BlackMisc::TDataTrait<BlackMisc::Aviation::CAircraftCategoryList>
    {
        //! First load is synchronous, countries is a small cache
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbaircraftcategorycache"; }
    };

    //! Trait for airport list
    struct TDbAirportCache : public BlackMisc::TDataTrait<BlackMisc::Aviation::CAirportList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }

        //! Key in data cache
        static const char *key() { return "dbairportcache"; }
    };

    //! Trait for ICAO reader base URL
    struct TDbIcaoReaderBaseUrl : public BlackMisc::TDataTrait<BlackMisc::Network::CUrl>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbicaoreaderurl"; }
    };

    //! Trait for ICAO reader base URL
    struct TDbModelReaderBaseUrl : public BlackMisc::TDataTrait<BlackMisc::Network::CUrl>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Key in data cache
        static const char *key() { return "dbmodelreaderurl"; }
    };
} // ns

#endif // guard
