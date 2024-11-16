// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H
#define SWIFT_MISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H

#include <QObject>
#include <QReadWriteLock>

#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/identifier.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CAircraftSituation;
        class CCallsign;
        class CTransponder;
    } // namespace aviation

    namespace simulation
    {
        //! For testing
        class SWIFT_MISC_EXPORT COwnAircraftProviderDummy : public QObject, public IOwnAircraftProvider
        {
            Q_OBJECT

        public:
            //! Constructor
            COwnAircraftProviderDummy() = default;

            //! Singleton
            static COwnAircraftProviderDummy *instance();

            //! \copydoc IOwnAircraftProvider::getOwnAircraft
            virtual CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc IOwnAircraftProvider::getOwnCallsign
            virtual aviation::CCallsign getOwnCallsign() const override;

            //! \copydoc IOwnAircraftProvider::getOwnComSystem
            virtual aviation::CComSystem getOwnComSystem(aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc IOwnAircraftProvider::getOwnTransponder
            virtual aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            virtual swift::misc::geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftSituation
            virtual swift::misc::aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
            virtual swift::misc::aviation::CAircraftParts getOwnAircraftParts() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
            virtual swift::misc::simulation::CAircraftModel getOwnAircraftModel() const override;

            //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
            virtual swift::misc::physical_quantities::CLength
            getDistanceToOwnAircraft(const swift::misc::geo::ICoordinateGeodetic &position) const override;

        public slots:
            //! \copydoc IOwnAircraftProvider::updateCockpit
            virtual bool updateCockpit(const swift::misc::aviation::CComSystem &com1,
                                       const swift::misc::aviation::CComSystem &com2,
                                       const swift::misc::aviation::CTransponder &transponder,
                                       const swift::misc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                                  swift::misc::aviation::CComSystem::ComUnit comUnit,
                                                  const swift::misc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateSelcal
            virtual bool updateSelcal(const swift::misc::aviation::CSelcal &selcal,
                                      const swift::misc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateOwnCallsign
            virtual bool updateOwnCallsign(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc IOwnAircraftProvider::updateOwnModel
            virtual bool updateOwnModel(const swift::misc::simulation::CAircraftModel &model) override;

            //! \copydoc IOwnAircraftProvider::updateOwnIcaoCodes
            virtual bool updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                            const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc IOwnAircraftProvider::updateOwnSituation
            virtual bool updateOwnSituation(const swift::misc::aviation::CAircraftSituation &situation) override;

            //! \copydoc IOwnAircraftProvider::updateOwnParts
            virtual bool updateOwnParts(const swift::misc::aviation::CAircraftParts &parts) override;

            //! \copydoc IOwnAircraftProvider::updateOwnCG
            virtual bool updateOwnCG(const swift::misc::physical_quantities::CLength &cg) override;

        private:
            swift::misc::simulation::CSimulatedAircraft m_ownAircraft;
            mutable QReadWriteLock m_lock; //!< lock for m_ownAircraft
        };
    } // namespace simulation
} // namespace swift::misc

#endif
