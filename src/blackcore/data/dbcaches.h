/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_DBCACHES
#define BLACKCORE_DATA_DBCACHES

#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/countrylist.h"

#include <QDateTime>
#include <QObject>

namespace BlackCore
{
    namespace Data
    {
        //! Trait for DB model cache
        struct DbModelCache : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
        {
            //! Defer loading
            static constexpr bool isDeferred() { return true; }

            //! Key in data cache
            static const char *key() { return "dbmodelcache"; }
        };

        //! Trait for DB distributor cache
        struct DbDistributorCache : public BlackMisc::CDataTrait<BlackMisc::Simulation::CDistributorList>
        {
            //! Defer loading (no currently small)
            static constexpr bool isDeferred() { return false; }

            //! Key in data cache
            static const char *key() { return "dbdistributorcache"; }
        };

        //! Trait for DB liveries
        struct DbLiveryCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CLiveryList>
        {
            //! Defer loading
            static constexpr bool isDeferred() { return true; }

            //! Key in data cache
            static const char *key() { return "dbliverycache"; }
        };

        //! Trait for DB airline ICAO codes
        struct DbAirlineIcaoCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CAirlineIcaoCodeList>
        {
            //! Defer loading
            static constexpr bool isDeferred() { return true; }

            //! Key in data cache
            static const char *key() { return "dbairlineicaocache"; }
        };


        //! Trait for DB aircraft ICAO codes
        struct DbAircraftIcaoCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CAircraftIcaoCodeList>
        {
            //! Defer loading
            static constexpr bool isDeferred() { return true; }

            //! Key in data cache
            static const char *key() { return "dbaircrafticaocache"; }
        };

        //! Trait for DB countries
        struct DbCountryCache : public BlackMisc::CDataTrait<BlackMisc::CCountryList>
        {
            //! Defer loading (no currently small)
            static constexpr bool isDeferred() { return false; }

            //! Key in data cache
            static const char *key() { return "dbcountrycache"; }
        };

        //! Trait for ICAO reader base URL
        struct DbIcaoReaderBaseUrl : public BlackMisc::CDataTrait<BlackMisc::Network::CUrl>
        {
            //! First load is synchronous
            static constexpr bool isPinned() { return true; }

            //! Key in data cache
            static const char *key() { return "dbicaoreaderurl"; }
        };

        //! Trait for ICAO reader base URL
        struct DbModelReaderBaseUrl : public BlackMisc::CDataTrait<BlackMisc::Network::CUrl>
        {
            //! First load is synchronous
            static constexpr bool isPinned() { return true; }

            //! Key in data cache
            static const char *key() { return "dbmodelreaderurl"; }
        };
    } // ns
} // ns

#endif // guard
