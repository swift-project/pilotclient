/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/weather/visibilitylayerlist.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc::Weather
{
    CVisibilityLayerList::CVisibilityLayerList(const CSequence<CVisibilityLayer> &other) :
        CSequence<CVisibilityLayer>(other)
    { }

    bool CVisibilityLayerList::containsBase(const CAltitude &base) const
    {
        return contains(&CVisibilityLayer::getBase, base);
    }

    CVisibilityLayer CVisibilityLayerList::findByBase(const CAltitude &base) const
    {
        return findFirstByOrDefault(&CVisibilityLayer::getBase, base);
    }

} // namespace
