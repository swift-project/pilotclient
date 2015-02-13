/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avaircraftsituationlist.h"
#include "predicates.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituationList::CAircraftSituationList()
        { }

        CAircraftSituationList::CAircraftSituationList(const CSequence<CAircraftSituation> &other) :
            CSequence<CAircraftSituation>(other)
        { }

        void CAircraftSituationList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftSituation>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftSituation>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftSituation>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftSituation>>();
            qRegisterMetaType<CAircraftSituationList>();
            qDBusRegisterMetaType<CAircraftSituationList>();
            registerMetaValueType<CAircraftSituationList>();
        }

    } // namespace
} // namespace
