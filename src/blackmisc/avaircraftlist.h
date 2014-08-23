/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFTLIST_H
#define BLACKMISC_AIRCRAFTLIST_H

#include "avaircraft.h"
#include "avcallsignlist.h"
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

            //! Find 0..n aircraft matching any of a set of callsigns
            CAircraftList findByCallsigns(const CCallsignList &callsigns) const;

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
