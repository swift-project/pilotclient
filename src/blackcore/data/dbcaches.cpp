/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbcaches.h"
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Data
    {
        const BlackMisc::Simulation::CAircraftModelList &DbModelCache::defaultValue()
        {
            static const CAircraftModelList ml;
            return ml;
        }

        const BlackMisc::Aviation::CAirlineIcaoCodeList &DbAirlineIcaoCache::defaultValue()
        {
            static const CAirlineIcaoCodeList al;
            return al;
        }

        const BlackMisc::Aviation::CAircraftIcaoCodeList &DbAircraftIcaoCache::defaultValue()
        {
            static const CAircraftIcaoCodeList al;
            return al;
        }

        const BlackMisc::CCountryList &DbCountryCache::defaultValue()
        {
            static const CCountryList cl;
            return cl;
        }

        const BlackMisc::Aviation::CLiveryList &DbLiveryCache::defaultValue()
        {
            static const CLiveryList ll;
            return ll;
        }

        const CDistributorList &DbDistributorCache::defaultValue()
        {
            static const CDistributorList dl;
            return dl;
        }

        const BlackMisc::Network::CUrl &DbIcaoReaderBaseUrl::defaultValue()
        {
            static const CUrl url;
            return url;
        }

        const CUrl &DbModelReaderBaseUrl::defaultValue()
        {
            static const CUrl url;
            return url;
        }
    } // ns
} // ns
