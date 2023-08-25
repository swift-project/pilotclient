// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/weather/visibilitylayerlist.h"

using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CVisibilityLayer, CVisibilityLayerList)

namespace BlackMisc::Weather
{
    CVisibilityLayerList::CVisibilityLayerList(const CSequence<CVisibilityLayer> &other) : CSequence<CVisibilityLayer>(other)
    {}

    bool CVisibilityLayerList::containsBase(const CAltitude &base) const
    {
        return contains(&CVisibilityLayer::getBase, base);
    }

    CVisibilityLayer CVisibilityLayerList::findByBase(const CAltitude &base) const
    {
        return findFirstByOrDefault(&CVisibilityLayer::getBase, base);
    }

} // namespace
