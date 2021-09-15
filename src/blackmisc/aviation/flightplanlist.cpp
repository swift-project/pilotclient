/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "flightplanlist.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"

#include <QString>
#include <QtGlobal>

namespace BlackMisc::Aviation
{
    CFlightPlanList::CFlightPlanList() { }

    CFlightPlanList::CFlightPlanList(const CSequence<CFlightPlan> &other) :
        CSequence<CFlightPlan>(other)
    { }
} // namespace
