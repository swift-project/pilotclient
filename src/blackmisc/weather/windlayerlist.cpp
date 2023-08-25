// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/weather/windlayerlist.h"

using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CWindLayer, CWindLayerList)

namespace BlackMisc::Weather
{
    CWindLayerList::CWindLayerList(const CSequence<CWindLayer> &other) : CSequence<CWindLayer>(other)
    {}

    bool CWindLayerList::containsLevel(const CAltitude &level) const
    {
        return contains(&CWindLayer::getLevel, level);
    }

    CWindLayer CWindLayerList::findByLevel(const CAltitude &level) const
    {
        return findFirstByOrDefault(&CWindLayer::getLevel, level);
    }

} // namespace
