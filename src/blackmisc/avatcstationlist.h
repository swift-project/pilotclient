/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_ATCSTATIONLIST_H
#define BLACKMISC_ATCSTATIONLIST_H

#include "nwuserlist.h"
#include "avatcstation.h"
#include "avcallsignlist.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object for a list of ATC stations.
         */
        class CAtcStationList : public CSequence<CAtcStation>
        {
        public:
            //! Default constructor.
            CAtcStationList();

            //! Construct from a base class object.
            CAtcStationList(const CSequence<CAtcStation> &other);

            //! Find 0..n stations by callsign
            CAtcStationList findByCallsign(const CCallsign &callsign) const;

            //! Find 0..n stations matching any of a set of callsigns
            CAtcStationList findByCallsigns(const CCallsignList &callsigns) const;

            //! Find first station by callsign, if not return given value / default
            CAtcStation findFirstByCallsign(const CCallsign &callsign, const CAtcStation &ifNotFound = CAtcStation()) const;

            //! Find 0..n stations within range of given coordinate
            CAtcStationList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! Find 0..n stations tune in frequency of COM unit (with 25kHt channel spacing
            CAtcStationList findIfComUnitTunedIn25KHz(const BlackMisc::Aviation::CComSystem &comUnit) const;

            //! All controllers (with valid data)
            BlackMisc::Network::CUserList getControllers() const;

            //! Update distances to coordinate, usually own aircraft's position
            void calculateDistancesToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            //! Merge with ATC station representing booking information.
            //! Both sides (booking, online station) will be updated.
            //! \remarks Can be used if the stored data in this list are online ATC stations
            int mergeWithBooking(CAtcStation &bookedAtcStation);

            //! Merge with the data from the VATSIM data file
            //! \remarks Can be used if the stored data in this list are VATSIM data file stations
            bool updateFromVatsimDataFileStation(CAtcStation &stationToBeUpdated) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAtcStation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAtcStation>)

#endif //guard
