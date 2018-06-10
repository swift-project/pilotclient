/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituationchangelist.h"

#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituationChangeList::CAircraftSituationChangeList() { }

        CAircraftSituationChangeList::CAircraftSituationChangeList(const CSequence<CAircraftSituationChange> &other) :
            CSequence<CAircraftSituationChange>(other)
        { }

        CAircraftSituationChangeList::CAircraftSituationChangeList(std::initializer_list<CAircraftSituationChange> il) :
            CSequence<CAircraftSituationChange>(il)
        { }
    } // namespace
} // namespace
