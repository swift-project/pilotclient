// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
