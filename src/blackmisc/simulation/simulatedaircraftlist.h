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
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/aviation/aircraftlist.h"
#include "blackmisc/geoobjectlist.h"
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
        class CSimulatedAircraftList :
            public BlackMisc::CSequence<CSimulatedAircraft>,
            public BlackMisc::Aviation::ICallsignObjectList<CSimulatedAircraft, CSimulatedAircraftList>,
            public BlackMisc::Geo::IGeoObjectWithRelativePositionList<CSimulatedAircraft, CSimulatedAircraftList>
        {
        public:
            //! Default constructor.
            CSimulatedAircraftList();

            //! Construct from a base class object.
            CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other);

            //! All pilots (with valid data)
            BlackMisc::Network::CUserList getPilots() const;

            //! Callsigns of aircraft with synchronized parts
            BlackMisc::Aviation::CCallsignList getCallsignsWithSyncronizedParts() const;

            //! Mark all aircraft as unrendered
            void markAllAsNotRendered();

            //! Mark as rendered
            int setRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered);

            //! Set aircraft parts
            int setAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

            //! Enabled?
            bool isEnabled(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Rendered?
            bool isRendered(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! To aircraft list
            BlackMisc::Aviation::CAircraftList toAircraftList() const;

            //! Register metadata
            static void registerMetadata();

        protected:
            //! Myself
            virtual const CSimulatedAircraftList &container() const { return *this; }

            //! Myself
            virtual CSimulatedAircraftList &container() { return *this; }

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatedAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CSimulatedAircraft>)

#endif //guard
