/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/flightplanlist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CFlightPlan, CFlightPlanList)

namespace BlackMisc::Aviation
{
    CFlightPlanList::CFlightPlanList() {}

    CFlightPlanList::CFlightPlanList(const CSequence<CFlightPlan> &other) : CSequence<CFlightPlan>(other)
    {}
} // namespace
