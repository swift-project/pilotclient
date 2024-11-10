// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_METARLIST_H
#define SWIFT_MISC_WEATHER_METARLIST_H

#include "misc/aviation/airporticaocode.h"
#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/mixin/mixincompare.h"
#include "misc/sequence.h"
#include "misc/weather/metar.h"

#include <QMetaType>
#include <QString>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::weather, CMetar, CMetarList)

namespace swift::misc::weather
{
    //! Sequence of Metars
    class SWIFT_MISC_EXPORT CMetarList :
        public CSequence<CMetar>,
        public swift::misc::mixin::MetaType<CMetarList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CMetarList)
        using CSequence::CSequence;

        //! Default constructor.
        CMetarList() = default;

        //! Construct from a base class object.
        CMetarList(const CSequence<CMetar> &other);

        //! METAR for ICAO code
        CMetar getMetarForAirport(const aviation::CAirportIcaoCode &icao) const;
    };

} // namespace

Q_DECLARE_METATYPE(swift::misc::weather::CMetarList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::weather::CMetar>)

#endif
