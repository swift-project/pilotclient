/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/weather/cloudlayerlist.h"

using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Weather, CCloudLayer, CCloudLayerList)

namespace BlackMisc::Weather
{
    CCloudLayerList::CCloudLayerList(const CSequence<CCloudLayer> &other) : CSequence<CCloudLayer>(other)
    {}

    bool CCloudLayerList::containsBase(const CAltitude &base) const
    {
        return contains(&CCloudLayer::getBase, base);
    }

    CCloudLayer CCloudLayerList::findByBase(const CAltitude &base) const
    {
        return findFirstByOrDefault(&CCloudLayer::getBase, base);
    }

} // namespace
