/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_METARLIST_H
#define BLACKMISC_WEATHER_METARLIST_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "blackmisc/weather/metar.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Weather
    {
        //! Sequence of Metars
        class BLACKMISC_EXPORT CMetarList :
            public CSequence<CMetar>,
            public BlackMisc::Mixin::MetaType<CMetarList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CMetarList)
            using CSequence::CSequence;

            //! Default constructor.
            CMetarList() = default;

            //! Construct from a base class object.
            CMetarList(const CSequence<CMetar> &other);

            //! METAR for ICAO code
            CMetar getMetarForAirport(const Aviation::CAirportIcaoCode &icao) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CMetarList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Weather::CMetar>)

#endif //guard
