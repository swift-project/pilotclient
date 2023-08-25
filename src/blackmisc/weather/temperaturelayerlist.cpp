// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/weather/temperaturelayerlist.h"

using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CTemperatureLayer, CTemperatureLayerList)

namespace BlackMisc::Weather
{
    CTemperatureLayerList::CTemperatureLayerList(const CSequence<CTemperatureLayer> &other) : CSequence<CTemperatureLayer>(other)
    {}

    bool CTemperatureLayerList::containsLevel(const CAltitude &level) const
    {
        return contains(&CTemperatureLayer::getLevel, level);
    }

    CTemperatureLayer CTemperatureLayerList::findByLevel(const CAltitude &level) const
    {
        return findFirstByOrDefault(&CTemperatureLayer::getLevel, level);
    }

} // namespace
