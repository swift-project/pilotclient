/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituationchangelist.h"

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftSituationChange, CAircraftSituationChangeList)

namespace BlackMisc::Aviation
{
    CAircraftSituationChangeList::CAircraftSituationChangeList() {}

    CAircraftSituationChangeList::CAircraftSituationChangeList(const CSequence<CAircraftSituationChange> &other) : CSequence<CAircraftSituationChange>(other)
    {}

    CAircraftSituationChange CAircraftSituationChangeList::frontOrNull() const
    {
        if (this->isEmpty()) { return CAircraftSituationChange::null(); }
        return this->front();
    }

    CAircraftSituationChange CAircraftSituationChangeList::backOrNull() const
    {
        if (this->isEmpty()) { return CAircraftSituationChange::null(); }
        return this->back();
    }

    CAircraftSituationChange CAircraftSituationChangeList::indexOrNull(int index) const
    {
        if (this->size() > index) { return (*this)[index]; }
        return CAircraftSituationChange::null();
    }
} // namespace
