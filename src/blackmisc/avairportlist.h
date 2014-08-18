/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_AIRPORTLIST_H
#define BLACKMISC_AIRPORTLIST_H

#include "avairport.h"
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
         * Value object for a list of airports.
         */
        class CAirportList : public CSequence<CAirport>
        {
        public:
            //! Default constructor.
            CAirportList();

            //! Construct from a base class object.
            CAirportList(const CSequence<CAirport> &other);

            //! Find 0..n airports by ICAO code
            CAirportList findByIcao(const CAirportIcao &icao) const;

            //! Replace or add based on same ICAO code
            void replaceOrAddByIcao(const CAirport &addedOrReplacedAirport);

            //! Find first station by callsign, if not return given value / default
            CAirport findFirstByIcao(const CAirportIcao &icao, const CAirport &ifNotFound = CAirport()) const;

            //! Find 0..n airports within range of given coordinate
            CAirportList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! Update distances to coordinate, usually own aircraft's position
            void calculcateDistanceAndBearingToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            //! Remove if outside given radius
            void removeIfOutsideRange(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &maxDistance, bool updateDistance);

            //! Sort by range from a given coordinate
            void sortByRange(const BlackMisc::Geo::CCoordinateGeodetic &position, bool updateDistance);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::fromQVariant
            virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirport>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAirport>)

#endif //guard
