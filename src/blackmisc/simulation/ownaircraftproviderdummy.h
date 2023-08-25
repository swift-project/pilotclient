// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H
#define BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifier.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QObject>
#include <QReadWriteLock>

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
        //! For testing
        class BLACKMISC_EXPORT COwnAircraftProviderDummy :
            public QObject,
            public IOwnAircraftProvider
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
            virtual Aviation::CCallsign getOwnCallsign() const override;

            //! \copydoc IOwnAircraftProvider::getOwnComSystem
            virtual Aviation::CComSystem getOwnComSystem(Aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc IOwnAircraftProvider::getOwnTransponder
            virtual Aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            virtual BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftSituation
            virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
            virtual BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
            virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const override;

            //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
            virtual BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const override;

        public slots:
            //! \copydoc IOwnAircraftProvider::updateCockpit
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateSelcal
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IOwnAircraftProvider::updateOwnCallsign
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc IOwnAircraftProvider::updateOwnModel
            virtual bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model) override;

            //! \copydoc IOwnAircraftProvider::updateOwnIcaoCodes
            virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc IOwnAircraftProvider::updateOwnSituation
            virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc IOwnAircraftProvider::updateOwnParts
            virtual bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts) override;

            //! \copydoc IOwnAircraftProvider::updateOwnCG
            virtual bool updateOwnCG(const BlackMisc::PhysicalQuantities::CLength &cg) override;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft;
            mutable QReadWriteLock m_lock; //!< lock for m_ownAircraft
        };
    } // namespace
} // namespace

#endif // guard
