/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_PRESENTWEATHERLIST_H
#define BLACKMISC_WEATHER_PRESENTWEATHERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/presentweather.h"
#include "presentweather.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Weather
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

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CPresentWeatherList)

#endif //guard
