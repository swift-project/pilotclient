/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATEDNAIRCRAFTLIST_H
#define BLACKMISC_SIMULATEDNAIRCRAFTLIST_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/avcallsignlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Value object encapsulating a list of aircraft.
        class CSimulatedAircraftList : public CSequence<CSimulatedAircraft>
        {
        public:
            //! Default constructor.
            CSimulatedAircraftList();

            //! Construct from a base class object.
            CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other);

            //! Find 0..n stations by callsign
            CSimulatedAircraftList findByCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Find 0..n aircraft matching any of a set of callsigns
            CSimulatedAircraftList findByCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const;

            //! Find the first aircraft by callsign, if none return given one
            CSimulatedAircraft findFirstByCallsign(const BlackMisc::Aviation::CCallsign &callsign, const CSimulatedAircraft &ifNotFound = CSimulatedAircraft()) const;

            //! Contains callsign?
            bool containsCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Incremental update or add aircraft
            int incrementalUpdateOrAdd(const BlackMisc::Simulation::CSimulatedAircraft &changedAircraft, const BlackMisc::CPropertyIndexVariantMap &changedValues);

            //! All pilots (with valid data)
            BlackMisc::Network::CUserList getPilots() const;

            /*!
             * Find 0..n stations within range of given coordinate
             * \param coordinate    other position
             * \param range         within range of other position
             * \return
             */
            CSimulatedAircraftList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatedAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CSimulatedAircraft>)

#endif //guard
