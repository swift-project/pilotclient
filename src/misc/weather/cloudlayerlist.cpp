// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/altitude.h"
#include "misc/mixin/mixincompare.h"
#include "misc/weather/cloudlayerlist.h"

using namespace swift::misc::aviation;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::weather, CCloudLayer, CCloudLayerList)

namespace swift::misc::weather
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
