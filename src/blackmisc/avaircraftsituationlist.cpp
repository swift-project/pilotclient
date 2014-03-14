#include "avaircraftsituationlist.h"

/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaircraftsituationlist.h"
#include "predicates.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituationList::CAircraftSituationList()
        {
        }

        /*
         * Construct from base class object
         */
        CAircraftSituationList::CAircraftSituationList(const CSequence<CAircraftSituation> &other) :
            CSequence<CAircraftSituation>(other)
        { }

        CAircraftSituationList CAircraftSituationList::findBefore(const QDateTime &dateTime) const
        {
            return findBy([&](const CAircraftSituation & situation)
            {
                return situation.getTimestamp() < dateTime;
            });
        }

        CAircraftSituationList CAircraftSituationList::findAfter(const QDateTime &dateTime) const
        {
            return findBy([&](const CAircraftSituation & situation)
            {
                return situation.getTimestamp() > dateTime;
            });
        }

        void CAircraftSituationList::removeBefore(const QDateTime &dateTime)
        {
            removeIf([&](const CAircraftSituation & situation)
            {
                return situation.getTimestamp() < dateTime;
            });
        }

        void CAircraftSituationList::removeOlderThan(double seconds)
        {
            removeIf([&](const CAircraftSituation & situation)
            {
                return situation.getTimestamp() < QDateTime::currentDateTimeUtc().addSecs(-seconds);
            });
        }

    } // namespace
} // namespace
