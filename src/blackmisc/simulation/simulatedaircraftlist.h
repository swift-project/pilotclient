/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATEDNAIRCRAFTLIST_H
#define BLACKMISC_SIMULATION_SIMULATEDNAIRCRAFTLIST_H

#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/variant.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace Simulation
    {
        class CAircraftModel;
        class CSimulatedAircraft;
    }

    namespace Simulation
    {
        //! Value object encapsulating a list of aircraft.
        class BLACKMISC_EXPORT CSimulatedAircraftList :
            public BlackMisc::CSequence<CSimulatedAircraft>,
            public BlackMisc::Aviation::ICallsignObjectList<CSimulatedAircraft, CSimulatedAircraftList>,
            public BlackMisc::Geo::IGeoObjectWithRelativePositionList<CSimulatedAircraft, CSimulatedAircraftList>,
            public BlackMisc::Mixin::MetaType<CSimulatedAircraftList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSimulatedAircraftList)

            //! Default constructor.
            CSimulatedAircraftList();

            //! Construct from a base class object.
            CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other);

            //! All pilots (with valid data)
            BlackMisc::Network::CUserList getPilots() const;

            //! Enabled / disabled aircraft
            CSimulatedAircraftList findByEnabled(bool enabled) const;

            //! Rendered / not rendered aircraft
            CSimulatedAircraftList findByRendered(bool rendered) const;

            //! VTOL / non VTOL aircraft
            CSimulatedAircraftList findByVtol(bool vtol) const;

            //! Callsigns of aircraft with synchronized parts
            BlackMisc::Aviation::CCallsignSet getCallsignsWithSynchronizedParts() const;

            //! Update aircraft with data from VATSIM data file
            //! \remarks The list used ("this") needs to contain the VATSIM data file objects
            bool updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUpdated) const;

            //! Mark all aircraft as unrendered
            void markAllAsNotRendered();

            //! Mark given callsign as rendered
            int setRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered, bool onlyFirst = true);

            //! Mark given callsign as enabled
            int setEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabled, bool onlyFirst = true);

            //! Set model
            int setAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftModel &model, bool onlyFirst = true);

            //! Set aircraft parts
            int setAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool onlyFirst = true);

            //! Set aircraft parts
            int setAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, bool onlyFirst = true);

            //! Set ground elevation
            int setGroundElevation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAltitude &elevation, bool onlyFirst = true);

            //! Enabled?
            bool isEnabled(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Rendered?
            bool isRendered(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Replace or add by callsign
            bool replaceOrAddByCallsign(const CSimulatedAircraft &aircraft);

            //! Number of enabled aircraft
            int countEnabled() const;

            //! Number of rendered aircraft
            int countRendered() const;

            //! Number of aircraft with parts
            int countAircraftPartsSyncronized() const;
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CSimulatedAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CSimulatedAircraft>)

#endif //guard
