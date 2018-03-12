/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDER_H
#define BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDER_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/provider.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/identifier.h"

#include <QObject>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CAircraftSituation;
        class CCallsign;
        class CTransponder;
    }

    namespace Simulation
    {
        //! Direct threadsafe in memory access to own aircraft
        class BLACKMISC_EXPORT IOwnAircraftProvider : public IProvider
        {
        public:
            //! Own aircraft
            //! \threadsafe
            virtual CSimulatedAircraft getOwnAircraft() const = 0;

            //! Own aircraft's position
            //! \threadsafe
            virtual BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const = 0;

            //! Own aircraft's situation
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const = 0;

            //! Own aircraft's parts
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const = 0;

            //! Own aircraft model
            //! \threadsafe
            virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const = 0;

            //! Distance to own aircraft
            //! \threadsafe
            virtual BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const = 0;

            //! Update aircraft's callsign
            //! \threadsafe
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

            //! Update ICAO data
            //! \threadsafe
            virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) = 0;

            //! Update model
            //! \threadsafe
            virtual bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model) = 0;

            //! Update own situation
            virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

            //! Update own parts
            virtual bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts) = 0;

            // ------------------------------------------------------------------------
            // cockpit / aircraft related updates, which can come from multiple sources
            // and are subject of roundtrips -> originator
            // ------------------------------------------------------------------------

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            bool updateCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) = 0;

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) = 0;

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) = 0;
        };

        //! Delegating class which can be directly used to access an \sa IOwnAircraftProvider instance
        class BLACKMISC_EXPORT COwnAircraftAware : public IProviderAware<IOwnAircraftProvider>
        {
        public:
            //! Set the provider
            void setOwnAircraftProvider(IOwnAircraftProvider *provider) { this->setProvider(provider); }

            //! \copydoc IOwnAircraftProvider::getOwnAircraft
            CSimulatedAircraft getOwnAircraft() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
            BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
            BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const;

            //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
            BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const;

            //! \copydoc IOwnAircraftProvider::updateCockpit(const BlackMisc::Simulation::CSimulatedAircraft &, const BlackMisc::CIdentifier &)
            bool updateCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateCockpit(const BlackMisc::Aviation::CComSystem &, const BlackMisc::Aviation::CComSystem &, const BlackMisc::Aviation::CTransponder &, const BlackMisc::CIdentifier &);
            bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateActiveComFrequency
            bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateSelcal
            bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateOwnCallsign
            bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! \copydoc IOwnAircraftProvider::updateOwnIcaoCodes
            bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoData, const Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! \copydoc IOwnAircraftProvider::updateOwnModel
            bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! \copydoc IOwnAircraftProvider::updateOwnSituation
            bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc IOwnAircraftProvider::updateOwnParts
            bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts);

        protected:
            //! Constructor
            COwnAircraftAware(IOwnAircraftProvider *ownAircraftProvider) : IProviderAware(ownAircraftProvider) { Q_ASSERT(ownAircraftProvider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IOwnAircraftProvider, "org.swift-project.blackmisc::simulation::iownaircraftprovider")

#endif // guard
