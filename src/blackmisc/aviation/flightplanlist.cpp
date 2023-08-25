// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/flightplanlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CFlightPlan, CFlightPlanList)

namespace BlackMisc::Aviation
{
    CFlightPlanList::CFlightPlanList() {}

    CFlightPlanList::CFlightPlanList(const CSequence<CFlightPlan> &other) : CSequence<CFlightPlan>(other)
    {}
} // namespace
