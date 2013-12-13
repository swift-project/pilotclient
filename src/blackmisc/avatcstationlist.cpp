/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avatcstationlist.h"
#include "predicates.h"
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Empty constructor
         */
        CAtcStationList::CAtcStationList() { }

        /*
         * Construct from base class object
         */
        CAtcStationList::CAtcStationList(const CSequence<CAtcStation> &other) :
            CSequence<CAtcStation>(other)
        { }

        /*
         * Register metadata
         */
        void CAtcStationList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAtcStation>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAtcStation>>();
            qRegisterMetaType<BlackMisc::CCollection<CAtcStation>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAtcStation>>();
            qRegisterMetaType<CAtcStationList>();
            qDBusRegisterMetaType<CAtcStationList>();
        }

        /*
         * Find by callsign
         */
        CAtcStationList CAtcStationList::findByCallsign(const CCallsign &callsign) const
        {
            return this->findBy(&CAtcStation::getCallsign, callsign);
        }

        /*
         * Stations within range
         */
        CAtcStationList CAtcStationList::findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
        {
            return this->findBy([&](const CAtcStation & atcStation)
            {
                return greatCircleDistance(atcStation, coordinate) <= range;
            });
        }

        /*
         * Distances to own plane
         */
        void CAtcStationList::calculateDistancesToPlane(const Geo::CCoordinateGeodetic &position)
        {
            std::for_each(this->begin(), this->end(), [ & ](CAtcStation &station)
            {
                station.calculcateDistanceToPlane(position);
            });
        }

        /*
         * Merge with booking
         */
        int  CAtcStationList::mergeWithBooking(CAtcStation &booking)
        {
            int c = 0;
            booking.setOnline(false); // reset

            for (auto i = this->begin(); i != this->end(); ++i)
            {
                CAtcStation currentOnline = *i;
                if (currentOnline.getCallsign() != booking.getCallsign()) continue;

                // from online to booking
                booking.setOnline(true);
                booking.setFrequency(currentOnline.getFrequency());

                // from booking to online
                if (!currentOnline.isBookedNow() && booking.hasValidBookingTimes())
                {
                    if (currentOnline.hasValidBookingTimes())
                    {
                        if (booking.isBookedNow())
                        {
                            // can't get any better
                            currentOnline.setBookedFromUntil(booking);
                        }
                        else
                        {
                            // we already have some booking dates
                            CTime timeDiffBooking = booking.bookedWhen();
                            CTime timeDiffOnline = currentOnline.bookedWhen();
                            if (timeDiffBooking.isNegativeWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                            {
                                // both in past
                                if (timeDiffBooking > timeDiffOnline)
                                    currentOnline.setBookedFromUntil(booking);
                            }
                            else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isPositiveWithEpsilonConsidered())
                            {
                                // both in future
                                if (timeDiffBooking < timeDiffOnline)
                                    currentOnline.setBookedFromUntil(booking);
                            }
                            else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                            {
                                // future booking is better than past booking
                                currentOnline.setBookedFromUntil(booking);
                            }
                        }
                    }
                    else
                    {
                        // no booking info so far
                        currentOnline.setBookedFromUntil(booking);
                    }
                }

                // both ways
                currentOnline.syncronizeControllerData(booking);
                if (currentOnline.hasValidDistance())
                    booking.setDistanceToPlane(currentOnline.getDistanceToPlane());
                else if (booking.hasValidDistance())
                    currentOnline.setDistanceToPlane(booking.getDistanceToPlane());

                // update
                *i = currentOnline;
                c++;
            }

            // normally 1 expected, as I should find one online station
            // for this booking
            return c;
        }

    } // namespace
} // namespace
