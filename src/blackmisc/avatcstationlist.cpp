/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avatcstationlist.h"
#include "predicates.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

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
         * Find first by callsign
         */
        CAtcStation CAtcStationList::findFirstByCallsign(const CCallsign &callsign, const CAtcStation &ifNotFound) const
        {
            CAtcStationList stations = findByCallsign(callsign);
            if (!stations.isEmpty()) return stations[0];
            return ifNotFound;
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
         * Find if on frequency of COM unit
         */
        CAtcStationList CAtcStationList::findIfComUnitTunedIn25KHz(const CComSystem &comUnit) const
        {
            return this->findBy([&](const CAtcStation & atcStation)
            {
                return atcStation.isComUnitTunedIn25KHz(comUnit);
            });
        }

        /*
         * Distances to own plane
         */
        void CAtcStationList::calculateDistancesToPlane(const Geo::CCoordinateGeodetic &position)
        {
            std::for_each(this->begin(), this->end(), [ & ](CAtcStation & station)
            {
                station.calculcateDistanceToPlane(position);
            });
        }

        /*
         * All pilots
         */
        CUserList CAtcStationList::getControllers() const
        {
            CUserList users;
            for (auto i = this->begin(); i != this->end(); ++i)
            {
                CAtcStation station = *i;
                if (station.getController().isValid()) users.push_back(station.getController());
            }
            return users;
        }


        /*
         * Merge with booking
         */
        int  CAtcStationList::mergeWithBooking(CAtcStation &bookedAtcStation)
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
                    bookedAtcStation.setDistanceToPlane(onlineAtcStation.getDistanceToPlane());
                else if (bookedAtcStation.hasValidDistance())
                    onlineAtcStation.setDistanceToPlane(bookedAtcStation.getDistanceToPlane());

                // update
                *i = onlineAtcStation;
                c++;
            }

            // normally 1 expected, as I should find
            // only one online station for this booking
            return c;
        }

        /*
         * Merge with VATSIM data file
         */
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
