/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_METARSET_H
#define BLACKMISC_WEATHER_METARSET_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "blackmisc/weather/metar.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Weather
    {
        //! Collection of Metars
        class BLACKMISC_EXPORT CMetarSet :
            public CCollection<CMetar>,
            public BlackMisc::Mixin::MetaType<CMetarSet>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CMetarSet)

            //! Default constructor.
            CMetarSet() = default;

            //! Construct from a base class object.
            CMetarSet(const CCollection<CMetar> &other);

            //! METAR for ICAO code
            CMetar getMetarForAirport(const Aviation::CAirportIcaoCode &icao) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CMetarSet)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Weather::CMetar>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Weather::CMetar>)

#endif //guard
