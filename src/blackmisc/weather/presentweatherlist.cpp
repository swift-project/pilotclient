// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/weather/presentweatherlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CPresentWeather, CPresentWeatherList)

namespace BlackMisc::Weather
{
    CPresentWeatherList::CPresentWeatherList(const CSequence<CPresentWeather> &other) : CSequence<CPresentWeather>(other)
    {}

} // namespace
