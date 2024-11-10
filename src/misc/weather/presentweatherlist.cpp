// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/presentweatherlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(swift::misc::weather, CPresentWeather, CPresentWeatherList)

namespace swift::misc::weather
{
    CPresentWeatherList::CPresentWeatherList(const CSequence<CPresentWeather> &other) : CSequence<CPresentWeather>(other)
    {}

} // namespace
