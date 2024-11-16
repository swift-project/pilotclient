// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATEDNAIRCRAFTLIST_H
#define SWIFT_MISC_SIMULATION_SIMULATEDNAIRCRAFTLIST_H

#include <QMetaType>

#include "misc/aviation/callsignobjectlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/collection.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/network/userlist.h"
#include "misc/sequence.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation, CSimulatedAircraft, CSimulatedAircraftList)

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftParts;
        class CCallsign;
    } // namespace aviation
    namespace simulation
    {
        class CAircraftModel;
        class CSimulatedAircraft;
    } // namespace simulation
    namespace simulation
    {
        //! Value object encapsulating a list of aircraft.
        class SWIFT_MISC_EXPORT CSimulatedAircraftList :
            public CSequence<CSimulatedAircraft>,
            public aviation::ICallsignObjectList<CSimulatedAircraft, CSimulatedAircraftList>,
            public geo::IGeoObjectWithRelativePositionList<CSimulatedAircraft, CSimulatedAircraftList>,
            public mixin::MetaType<CSimulatedAircraftList>
        {
        public:
            SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CSimulatedAircraftList)
            using CSequence::CSequence;

            //! Default constructor.
            CSimulatedAircraftList();

            //! Construct from a base class object.
            CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other);

            //! All pilots (with valid data)
            network::CUserList getPilots() const;

            //! Get all models
            CAircraftModelList getModels() const;

            //! Enabled / disabled aircraft
            Q_REQUIRED_RESULT CSimulatedAircraftList findByEnabled(bool enabled) const;

            //! Rendered / not rendered aircraft
            Q_REQUIRED_RESULT CSimulatedAircraftList findByRendered(bool rendered) const;

            //! VTOL / non VTOL aircraft
            CSimulatedAircraftList findByVtol(bool vtol) const;

            //! Callsigns of aircraft with synchronized parts
            aviation::CCallsignSet getCallsignsWithSynchronizedParts() const;

            //! Update aircraft with data from VATSIM data file
            //! \remarks The list used ("this") needs to contain the VATSIM data file objects
            bool updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUpdated) const;

            //! Mark all aircraft as unrendered
            void markAllAsNotRendered();

            //! Mark given callsign as rendered
            int setRendered(const aviation::CCallsign &callsign, bool rendered, bool onlyFirst = true);

            //! Set center of gravity
            int setCG(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg,
                      bool onlyFirst = true);

            //! Mark given callsign as enabled
            int setEnabled(const aviation::CCallsign &callsign, bool enabled, bool onlyFirst);

            //! Mark as fast position enabled
            int setFastPositionUpdates(const aviation::CCallsign &callsign, bool fastPositions, bool onlyFirst = true);

            //! Set model
            int setAircraftModel(const aviation::CCallsign &callsign, const CAircraftModel &model,
                                 bool onlyFirst = true);

            //! Set aircraft parts and mark as synchronized
            int setAircraftPartsSynchronized(const aviation::CCallsign &callsign, const aviation::CAircraftParts &parts,
                                             bool onlyFirst = true);

            //! Set aircraft situation
            int setAircraftSituation(const aviation::CCallsign &callsign, const aviation::CAircraftSituation &situation,
                                     bool onlyFirst = true);

            //! Set ground elevation
            int setGroundElevationChecked(const aviation::CCallsign &callsign, const geo::CElevationPlane &elevation,
                                          aviation::CAircraftSituation::GndElevationInfo info, bool onlyFirst = true);

            //! Enabled?
            bool isEnabled(const aviation::CCallsign &callsign) const;

            //! Rendered?
            bool isRendered(const aviation::CCallsign &callsign) const;

            //! Replace or add by callsign
            bool replaceOrAddByCallsign(const CSimulatedAircraft &aircraft);

            //! Number of enabled aircraft
            int countEnabled() const;

            //! Number of rendered aircraft
            int countRendered() const;

            //! Number of aircraft with parts
            int countAircraftPartsSynchronized() const;

            //! Same as sortByDistanceToReferencePosition, but consider callsign and rendered as secondary criteria
            void sortByDistanceToReferencePositionRenderedCallsign();
        };

        //! Aircraft per callsign
        using CSimulatedAircraftPerCallsign = QHash<aviation::CCallsign, CSimulatedAircraft>;

    } // namespace simulation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatedAircraftList)
Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatedAircraftPerCallsign)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::CSimulatedAircraft>)

#endif // guard
