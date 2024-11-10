// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/interpolation/interpolationsetuplist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation, CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList)

namespace swift::misc::simulation
{
    CInterpolationSetupList::CInterpolationSetupList() {}

    CInterpolationSetupList::CInterpolationSetupList(const CInterpolationAndRenderingSetupPerCallsign &setup)
    {
        this->push_back(setup);
    }

    CInterpolationSetupList::CInterpolationSetupList(const CSequence<CInterpolationAndRenderingSetupPerCallsign> &other) : CSequence<CInterpolationAndRenderingSetupPerCallsign>(other)
    {}

} // namespace
