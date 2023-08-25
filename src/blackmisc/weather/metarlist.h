// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_METARLIST_H
#define BLACKMISC_WEATHER_METARLIST_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/metar.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CMetar, CMetarList)

namespace BlackMisc::Weather
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

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CMetarList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Weather::CMetar>)

#endif // guard
