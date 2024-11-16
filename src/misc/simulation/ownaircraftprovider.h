// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_OWNAIRCRAFTPROVIDER_H
#define SWIFT_MISC_SIMULATION_OWNAIRCRAFTPROVIDER_H

#include <QObject>
#include <QtGlobal>

#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/identifier.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/provider.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CAircraftSituation;
    } // namespace aviation

    namespace simulation
    {
        //! Direct threadsafe in memory access to own aircraft
        class SWIFT_MISC_EXPORT IOwnAircraftProvider : public IProvider
        {
        public:
            //! Own aircraft
            //! \threadsafe
            virtual CSimulatedAircraft getOwnAircraft() const = 0;

            //! Own aircraft's callsign
            //! \threadsafe
            virtual aviation::CCallsign getOwnCallsign() const = 0;

            //! Own aircraft COM unit
            //! \threadsafe
            virtual aviation::CComSystem getOwnComSystem(aviation::CComSystem::ComUnit unit) const = 0;

            //! Own aircraft XPDR
            //! \threadsafe
            virtual aviation::CTransponder getOwnTransponder() const = 0;

            //! Own aircraft's position
            //! \threadsafe
            virtual geo::CCoordinateGeodetic getOwnAircraftPosition() const = 0;

            //! Own aircraft's situation
            //! \threadsafe
            virtual aviation::CAircraftSituation getOwnAircraftSituation() const = 0;

            //! Own aircraft's parts
            //! \threadsafe
            virtual aviation::CAircraftParts getOwnAircraftParts() const = 0;

            //! Own aircraft model
            //! \threadsafe
            virtual swift::misc::simulation::CAircraftModel getOwnAircraftModel() const = 0;

            //! Distance to own aircraft
            //! \threadsafe
            virtual swift::misc::physical_quantities::CLength getDistanceToOwnAircraft(const swift::misc::geo::ICoordinateGeodetic &position) const = 0;

            //! Update aircraft's callsign
            //! \threadsafe
            virtual bool updateOwnCallsign(const aviation::CCallsign &callsign) = 0;

            //! Update ICAO data
            //! \threadsafe
            virtual bool updateOwnIcaoCodes(const aviation::CAircraftIcaoCode &aircraftIcaoCode, const aviation::CAirlineIcaoCode &airlineIcaoCode) = 0;

            //! Update model
            //! \threadsafe
            virtual bool updateOwnModel(const swift::misc::simulation::CAircraftModel &model) = 0;

            //! Update own situation
            virtual bool updateOwnSituation(const aviation::CAircraftSituation &situation) = 0;

            //! Update own parts
            virtual bool updateOwnParts(const aviation::CAircraftParts &parts) = 0;

            //! Update own aircraft's CG (aka vertical offset)
            virtual bool updateOwnCG(const physical_quantities::CLength &cg) = 0;

            // ------------------------------------------------------------------------
            // cockpit / aircraft related updates, which can come from multiple sources
            // and are subject of roundtrips -> originator
            // ------------------------------------------------------------------------

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            bool updateCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator);

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateCockpit(const aviation::CComSystem &com1, const aviation::CComSystem &com2, const aviation::CTransponder &transponder, const swift::misc::CIdentifier &originator) = 0;

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency, aviation::CComSystem::ComUnit comUnit, const swift::misc::CIdentifier &originator) = 0;

            //! Update cockpit, but only send signals when applicable
            //! \threadsafe
            virtual bool updateSelcal(const aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator) = 0;
        };

        //! Delegating class which can be directly used to access an \sa IOwnAircraftProvider instance
        class SWIFT_MISC_EXPORT COwnAircraftAware : public IProviderAware<IOwnAircraftProvider>
        {
            virtual void anchor();

        public:
            //! Set the provider
            void setOwnAircraftProvider(IOwnAircraftProvider *provider) { this->setProvider(provider); }

            //! \copydoc IOwnAircraftProvider::getOwnAircraft
            CSimulatedAircraft getOwnAircraft() const;

            //! \copydoc IOwnAircraftProvider::getOwnCallsign
            aviation::CCallsign getOwnCallsign() const;

            //! \copydoc IOwnAircraftProvider::getOwnComSystem
            aviation::CComSystem getOwnComSystem(aviation::CComSystem::ComUnit unit) const;

            //! \copydoc IOwnAircraftProvider::getOwnTransponder
            aviation::CTransponder getOwnTransponder() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            swift::misc::geo::CCoordinateGeodetic getOwnAircraftPosition() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            aviation::CAircraftSituation getOwnAircraftSituation() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
            aviation::CAircraftParts getOwnAircraftParts() const;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
            swift::misc::simulation::CAircraftModel getOwnAircraftModel() const;

            //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
            swift::misc::physical_quantities::CLength getDistanceToOwnAircraft(const swift::misc::geo::ICoordinateGeodetic &position) const;

            //! \copydoc IOwnAircraftProvider::updateCockpit(const swift::misc::simulation::CSimulatedAircraft &, const swift::misc::CIdentifier &)
            bool updateCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateCockpit(const aviation::CComSystem &, const aviation::CComSystem &, const aviation::CTransponder &, const swift::misc::CIdentifier &);
            bool updateCockpit(const aviation::CComSystem &com1, const aviation::CComSystem &com2, const aviation::CTransponder &transponder, const swift::misc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateActiveComFrequency
            bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency, aviation::CComSystem::ComUnit comUnit, const swift::misc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateSelcal
            bool updateSelcal(const aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator);

            //! \copydoc IOwnAircraftProvider::updateOwnCallsign
            bool updateOwnCallsign(const aviation::CCallsign &callsign);

            //! \copydoc IOwnAircraftProvider::updateOwnIcaoCodes
            bool updateOwnIcaoCodes(const aviation::CAircraftIcaoCode &aircraftIcaoData, const aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! \copydoc IOwnAircraftProvider::updateOwnModel
            bool updateOwnModel(const swift::misc::simulation::CAircraftModel &model);

            //! \copydoc IOwnAircraftProvider::updateOwnSituation
            bool updateOwnSituation(const aviation::CAircraftSituation &situation);

            //! \copydoc IOwnAircraftProvider::updateOwnParts
            bool updateOwnParts(const aviation::CAircraftParts &parts);

            //! \copydoc IOwnAircraftProvider::updateOwnCG
            bool updateOwnCG(const physical_quantities::CLength &cg);

        protected:
            //! Constructor
            COwnAircraftAware(IOwnAircraftProvider *ownAircraftProvider) : IProviderAware(ownAircraftProvider) { Q_ASSERT(ownAircraftProvider); }
        };
    } // namespace simulation
} // namespace swift::misc

Q_DECLARE_INTERFACE(swift::misc::simulation::IOwnAircraftProvider, "org.swift-project.misc::simulation::iownaircraftprovider")

#endif
