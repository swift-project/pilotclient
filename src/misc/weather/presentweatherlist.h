// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_PRESENTWEATHERLIST_H
#define SWIFT_MISC_WEATHER_PRESENTWEATHERLIST_H

#include "misc/swiftmiscexport.h"
#include "misc/sequence.h"
#include "misc/weather/presentweather.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::weather, CPresentWeather, CPresentWeatherList)

namespace swift::misc::weather
{
    /*!
     * Value object encapsulating a list of present weathers
     */
    class SWIFT_MISC_EXPORT CPresentWeatherList :
        public CSequence<CPresentWeather>,
        public swift::misc::mixin::MetaType<CPresentWeatherList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CPresentWeatherList)
        using CSequence::CSequence;

        //! Default constructor.
        CPresentWeatherList() = default;

        //! Construct from a base class object.
        CPresentWeatherList(const CSequence<CPresentWeather> &other);
    };

} // namespace

Q_DECLARE_METATYPE(swift::misc::weather::CPresentWeatherList)

#endif // guard
