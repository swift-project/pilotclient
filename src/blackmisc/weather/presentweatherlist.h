// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_PRESENTWEATHERLIST_H
#define BLACKMISC_WEATHER_PRESENTWEATHERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/presentweather.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CPresentWeather, CPresentWeatherList)

namespace BlackMisc::Weather
{
    /*!
     * Value object encapsulating a list of present weathers
     */
    class BLACKMISC_EXPORT CPresentWeatherList :
        public CSequence<CPresentWeather>,
        public BlackMisc::Mixin::MetaType<CPresentWeatherList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CPresentWeatherList)
        using CSequence::CSequence;

        //! Default constructor.
        CPresentWeatherList() = default;

        //! Construct from a base class object.
        CPresentWeatherList(const CSequence<CPresentWeather> &other);
    };

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CPresentWeatherList)

#endif // guard
