// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/range.h"
#include "blackmisc/weather/weatherdataplugininfo.h"
#include "blackmisc/weather/weatherdataplugininfolist.h"

#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CWeatherDataPluginInfo, CWeatherDataPluginInfoList)

namespace BlackMisc::Weather
{

    CWeatherDataPluginInfoList::CWeatherDataPluginInfoList() {}

    QStringList CWeatherDataPluginInfoList::toStringList(bool i18n) const
    {
        return this->transform([i18n](const CWeatherDataPluginInfo &info) { return info.toQString(i18n); });
    }

} // namespace
