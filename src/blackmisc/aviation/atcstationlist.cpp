/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/network/user.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"

#include <QString>
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {
        CAtcStationList::CAtcStationList() { }

        CAtcStationList::CAtcStationList(const CSequence<CAtcStation> &other) :
            CSequence<CAtcStation>(other)
        { }

        CAtcStationList CAtcStationList::findIfComUnitTunedIn25KHz(const CComSystem &comUnit) const
        {
            return this->findBy([&](const CAtcStation & atcStation)
            {
                return atcStation.isComUnitTunedIn25KHz(comUnit);
            });
        }

        CAtcStationList CAtcStationList::stationsWithValidVoiceRoom() const
        {
            return this->findBy(&CAtcStation::hasValidVoiceRoom, true);
        }

        CAtcStationList CAtcStationList::stationsWithValidFrequency() const
        {
            return this->findBy(&CAtcStation::hasValidFrequency, true);
        }

        CUserList CAtcStationList::getControllers() const
        {
            return this->findBy(Predicates::MemberValid(&CAtcStation::getController)).transform(Predicates::MemberTransform(&CAtcStation::getController));
        }

        int CAtcStationList::removeIfOutsideRange()
        {
            return this->removeIf(&CAtcStation::isInRange, false);
        }

        int CAtcStationList::synchronizeWithBookedStation(CAtcStation &bookedAtcStation)
        {
            int c = 0;
            bookedAtcStation.setOnline(false); // reset
            if (this->isEmpty()) return 0;

            for (auto i = this->begin(); i != this->end(); ++i)
            {
                if (i->getCallsign() != bookedAtcStation.getCallsign()) { continue; }
                i->synchronizeWithBookedStation(bookedAtcStation);
                c++;
            }

            // normally 1 expected, as I should find
            // only one online station for this booking
            Q_ASSERT_X(c == 0 || c == 1, Q_FUNC_INFO, "Found >1 matching station");
            return c;
        }
    } // namespace
} // namespace
