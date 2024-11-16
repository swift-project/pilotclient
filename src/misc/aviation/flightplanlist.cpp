// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/flightplanlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CFlightPlan, CFlightPlanList)

namespace swift::misc::aviation
{
    CFlightPlanList::CFlightPlanList() {}

    CFlightPlanList::CFlightPlanList(const CSequence<CFlightPlan> &other) : CSequence<CFlightPlan>(other) {}
} // namespace swift::misc::aviation
