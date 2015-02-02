/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindexallclasses.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        /*
         * Default constructor
         */
        CSimulatedAircraftList::CSimulatedAircraftList() { }

        /*
         * Construct from base class object
         */
        CSimulatedAircraftList::CSimulatedAircraftList(const CSequence<CSimulatedAircraft> &other) :
            CSequence<CSimulatedAircraft>(other)
        { }

        /*
         * Register metadata
         */
        void CSimulatedAircraftList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CSimulatedAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CSimulatedAircraft>>();
            qRegisterMetaType<BlackMisc::CCollection<CSimulatedAircraft>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CSimulatedAircraft>>();
            qRegisterMetaType<CSimulatedAircraftList>();
            qDBusRegisterMetaType<CSimulatedAircraftList>();
            registerMetaValueType<CSimulatedAircraftList>();
        }

        /*
         * All pilots
         */
        CUserList CSimulatedAircraftList::getPilots() const
        {
            return this->findBy(Predicates::MemberValid(&CSimulatedAircraft::getPilot)).transform(Predicates::MemberTransform(&CSimulatedAircraft::getPilot));
        }

        CAircraftList CSimulatedAircraftList::toAircraftList() const
        {
            CAircraftList al;
            for (const CSimulatedAircraft &aircraft : (*this))
            {
                al.push_back(aircraft);
            }
            return al;
        }

    } // namespace
} // namespace
