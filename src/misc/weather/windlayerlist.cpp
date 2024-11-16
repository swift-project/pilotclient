// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/windlayerlist.h"

#include "misc/aviation/altitude.h"
#include "misc/mixin/mixincompare.h"

using namespace swift::misc::aviation;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::weather, CWindLayer, CWindLayerList)

namespace swift::misc::weather
{
    CWindLayerList::CWindLayerList(const CSequence<CWindLayer> &other) : CSequence<CWindLayer>(other) {}

    bool CWindLayerList::containsLevel(const CAltitude &level) const { return contains(&CWindLayer::getLevel, level); }

    CWindLayer CWindLayerList::findByLevel(const CAltitude &level) const
    {
        return findFirstByOrDefault(&CWindLayer::getLevel, level);
    }

} // namespace swift::misc::weather
