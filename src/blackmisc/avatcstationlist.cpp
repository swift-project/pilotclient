/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avatcstationlist.h"
#include "predicates.h"

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

        void CAtcStationList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAtcStation>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAtcStation>>();
            qRegisterMetaType<BlackMisc::CCollection<CAtcStation>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAtcStation>>();
            qRegisterMetaType<CAtcStationList>();
            qDBusRegisterMetaType<CAtcStationList>();
            registerMetaValueType<CAtcStationList>();
        }

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

        CUserList CAtcStationList::getControllers() const
        {
            return this->findBy(Predicates::MemberValid(&CAtcStation::getController)).transform(Predicates::MemberTransform(&CAtcStation::getController));
        }

        int CAtcStationList::mergeWithBooking(CAtcStation &bookedAtcStation)
        {
            int c = 0;
            bookedAtcStation.setOnline(false); // reset
            if (this->isEmpty()) return 0;

            for (auto i = this->begin(); i != this->end(); ++i)
            {
                CAtcStation onlineAtcStation = *i;
                if (onlineAtcStation.getCallsign() != bookedAtcStation.getCallsign()) continue;

                // from online to booking
                bookedAtcStation.setOnline(true);
                bookedAtcStation.setFrequency(onlineAtcStation.getFrequency());

                // Logoff Zulu Time set?
                // comes directly from the online controller and is most likely more accurate
                if (!onlineAtcStation.getBookedUntilUtc().isNull())
                    bookedAtcStation.setBookedUntilUtc(onlineAtcStation.getBookedUntilUtc());

                // from booking to online
                if (!onlineAtcStation.isBookedNow() && bookedAtcStation.hasValidBookingTimes())
                {
                    if (onlineAtcStation.hasValidBookingTimes())
                    {
                        if (bookedAtcStation.isBookedNow())
                        {
                            // can't get any better
                            onlineAtcStation.setBookedFromUntil(bookedAtcStation);
                        }
                        else
                        {
                            // we already have some booking dates
                            CTime timeDiffBooking = bookedAtcStation.bookedWhen();
                            CTime timeDiffOnline = onlineAtcStation.bookedWhen();
                            if (timeDiffBooking.isNegativeWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                            {
                                // both in past
                                if (timeDiffBooking > timeDiffOnline)
                                    onlineAtcStation.setBookedFromUntil(bookedAtcStation);
                            }
                            else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isPositiveWithEpsilonConsidered())
                            {
                                // both in future
                                if (timeDiffBooking < timeDiffOnline)
                                    onlineAtcStation.setBookedFromUntil(bookedAtcStation);
                            }
                            else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                            {
                                // future booking is better than past booking
                                onlineAtcStation.setBookedFromUntil(bookedAtcStation);
                            }
                        }
                    }
                    else
                    {
                        // no booking info so far
                        onlineAtcStation.setBookedFromUntil(bookedAtcStation);
                    }
                }

                // both ways
                onlineAtcStation.syncronizeControllerData(bookedAtcStation);
                if (onlineAtcStation.hasValidDistance())
                {
                    bookedAtcStation.setDistanceToOwnAircraft(onlineAtcStation.getDistanceToOwnAircraft());
                    bookedAtcStation.setBearingToOwnAircraft(onlineAtcStation.getBearingToOwnAIrcraft());
                }
                else if (bookedAtcStation.hasValidDistance())
                {
                    onlineAtcStation.setDistanceToOwnAircraft(bookedAtcStation.getDistanceToOwnAircraft());
                    onlineAtcStation.setBearingToOwnAircraft(bookedAtcStation.getBearingToOwnAIrcraft());
                }

                // update
                *i = onlineAtcStation;
                c++;
            }

            // normally 1 expected, as I should find
            // only one online station for this booking
            Q_ASSERT(c == 0 || c == 1);
            return c;
        }

        bool CAtcStationList::updateFromVatsimDataFileStation(CAtcStation &stationToBeUpdated) const
        {
            if (this->isEmpty()) return false;
            if (stationToBeUpdated.hasValidRealName() && stationToBeUpdated.hasValidId() && stationToBeUpdated.hasValidFrequency()) return 0;

            CAtcStation dataFileStation = this->findFirstByCallsign(stationToBeUpdated.getCallsign());
            if (dataFileStation.getCallsign().isEmpty()) return false; // not found

            if (!stationToBeUpdated.hasValidRealName() || !stationToBeUpdated.hasValidId())
            {
                CUser user = stationToBeUpdated.getController();
                if (!stationToBeUpdated.hasValidRealName()) user.setRealName(dataFileStation.getControllerRealName());
                if (!stationToBeUpdated.hasValidId()) user.setId(dataFileStation.getControllerId());
                stationToBeUpdated.setController(user);
            }

            if (!stationToBeUpdated.hasValidFrequency())
            {
                stationToBeUpdated.setFrequency(dataFileStation.getFrequency());
            }
            return true;
        }

    } // namespace
} // namespace
