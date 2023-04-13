/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
