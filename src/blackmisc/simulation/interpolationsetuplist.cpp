// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolationsetuplist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Simulation, CInterpolationAndRenderingSetupPerCallsign, CInterpolationSetupList)

namespace BlackMisc::Simulation
{
    CInterpolationSetupList::CInterpolationSetupList() {}

    CInterpolationSetupList::CInterpolationSetupList(const CInterpolationAndRenderingSetupPerCallsign &setup)
    {
        this->push_back(setup);
    }

    CInterpolationSetupList::CInterpolationSetupList(const CSequence<CInterpolationAndRenderingSetupPerCallsign> &other) : CSequence<CInterpolationAndRenderingSetupPerCallsign>(other)
    {}

} // namespace
