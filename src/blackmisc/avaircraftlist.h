/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFTLIST_H
#define BLACKMISC_AIRCRAFTLIST_H

#include "avaircraft.h"
#include "nwuserlist.h"
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
         * Value object encapsulating a list of aircrafts.
         */
        class CAircraftList : public CSequence<CAircraft>
        {
        public:
            //! Default constructor.
            CAircraftList();

            //! Construct from a base class object.
            CAircraftList(const CSequence<CAircraft> &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Find 0..n stations by callsign
            CAircraftList findByCallsign(const CCallsign &callsign) const;

            //! Find the first aircraft by callsign, if none return given one
            CAircraft findFirstByCallsign(const CCallsign &callsign, const CAircraft &ifNotFound = CAircraft()) const;

            //! All pilots (with valid data)
            BlackMisc::Network::CUserList getPilots() const;

            /*!
             * Find 0..n stations within range of given coordinate
             * \param coordinate    other position
             * \param range         within range of other position
             * \return
             */
            CAircraftList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! Register metadata
            static void registerMetadata();

            //! \brief Update aircraft with data from VATSIM data file
            //! \remarks The list used needs to contain the VATSIM data file objects
            bool updateWithVatsimDataFileData(CAircraft &aircraftToBeUpdated) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraft>)

#endif //guard
