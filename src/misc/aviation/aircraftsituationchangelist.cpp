// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftsituationchangelist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftSituationChange, CAircraftSituationChangeList)

namespace swift::misc::aviation
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
