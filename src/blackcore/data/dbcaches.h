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
            //! Default value
            static const BlackMisc::Simulation::CAircraftModelList &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbmodelcache"; }
        };

        //! Trait for DB airline ICAO codes
        struct DbAirlineIcaoCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CAirlineIcaoCodeList>
        {
            //! Default value
            static const BlackMisc::Aviation::CAirlineIcaoCodeList &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbairlineicaocache"; }
        };


        //! Trait for DB aircraft ICAO codes
        struct DbAircraftIcaoCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CAircraftIcaoCodeList>
        {
            //! Default value
            static const BlackMisc::Aviation::CAircraftIcaoCodeList &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbaircrafticaocache"; }
        };

        //! Trait for DB countries
        struct DbCountryCache : public BlackMisc::CDataTrait<BlackMisc::CCountryList>
        {
            //! Default value
            static const BlackMisc::CCountryList &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbcountrycache"; }
        };

        //! Trait for DB liveries
        struct DbLiveryCache : public BlackMisc::CDataTrait<BlackMisc::Aviation::CLiveryList>
        {
            //! Default value
            static const BlackMisc::Aviation::CLiveryList &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbliverycache"; }
        };

        //! Trait for ICAO reader base URL
        struct DbIcaoReaderBaseUrl : public BlackMisc::CDataTrait<BlackMisc::Network::CUrl>
        {
            //! Default value
            static const BlackMisc::Network::CUrl &defaultValue();

            //! Key in data cache
            static const char *key() { return "dbicaoreaderurl"; }
        };


    } // ns
} // ns

#endif // guard
