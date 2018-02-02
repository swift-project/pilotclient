/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftpartslist.h"

#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftPartsList::CAircraftPartsList() { }

        CAircraftPartsList::CAircraftPartsList(const CSequence<CAircraftParts> &other) :
            CSequence<CAircraftParts>(other)
        { }

        CAircraftPartsList::CAircraftPartsList(std::initializer_list<CAircraftParts> il) :
            CSequence<CAircraftParts>(il)
        { }
    } // namespace
} // namespace
