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
         * Find by callsign
         */
        CSimulatedAircraftList CSimulatedAircraftList::findByCallsign(const CCallsign &callsign) const
        {
            return this->findBy(&CSimulatedAircraft::getCallsign, callsign);
        }

        /*
         * Find by callsigns
         */
        CSimulatedAircraftList CSimulatedAircraftList::findByCallsigns(const CCallsignList &callsigns) const
        {
            return this->findBy(Predicates::MemberIsAnyOf(&CSimulatedAircraft::getCallsign, callsigns));
        }

        /*
         * Find by callsign
         */
        CSimulatedAircraft CSimulatedAircraftList::findFirstByCallsign(const CCallsign &callsign, const CSimulatedAircraft &ifNotFound) const
        {
            return this->findByCallsign(callsign).frontOrDefault(ifNotFound);
        }

        /*
         * Contains callsign?
         */
        bool CSimulatedAircraftList::containsCallsign(const CCallsign &callsign) const
        {
            return this->contains(&CSimulatedAircraft::getCallsign, callsign);
        }

        int CSimulatedAircraftList::incrementalUpdateOrAdd(const CSimulatedAircraft &toChangeAircraft, const CPropertyIndexVariantMap &changedValues)
        {
            int c;
            const CCallsign cs = toChangeAircraft.getCallsign();
            if (this->containsCallsign(cs))
            {
                if (changedValues.isEmpty()) { return 0; }
                c = this->applyIf(&CSimulatedAircraft::getCallsign, cs, changedValues);
            }
            else
            {
                c = 1;
                if (changedValues.isEmpty())
                {
                    this->push_back(toChangeAircraft);
                }
                else
                {
                    CSimulatedAircraft addAircraft(toChangeAircraft);
                    addAircraft.apply(changedValues);
                    this->push_back(addAircraft);
                }
            }
            return c;
        }

        /*
         * All pilots
         */
        CUserList CSimulatedAircraftList::getPilots() const
        {
            return this->findBy(Predicates::MemberValid(&CSimulatedAircraft::getPilot)).transform(Predicates::MemberTransform(&CSimulatedAircraft::getPilot));
        }

        /*
         * Aircrafts within range
         */
        CSimulatedAircraftList CSimulatedAircraftList::findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
        {
            return this->findBy([&](const CSimulatedAircraft & aircraft)
            {
                return BlackMisc::Geo::greatCircleDistance(aircraft, coordinate) <= range;
            });
        }

    } // namespace
} // namespace
