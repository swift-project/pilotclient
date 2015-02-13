/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMDIRECTACCESSOWNAIRCRAFT_H
#define BLACKMISC_SIMDIRECTACCESSOWNAIRCRAFT_H

#include "blackmisc/simulation/simulatedaircraft.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to own aircraft
        class IOwnAircraftProviderReadOnly
        {
        public:
            //! Own aircraft as reference
            //! \note in memory, not thread safe!
            virtual const CSimulatedAircraft &ownAircraft() const = 0;

            //! Own aircraft as copy
            //! \note not hread safe!
            virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const = 0;

            //! Destructor
            virtual ~IOwnAircraftProviderReadOnly() {}
        };

        //! Direct in memory access to own aircraft
        class IOwnAircraftProvider : public IOwnAircraftProviderReadOnly
        {
        public:
            //! Own aircraft
            virtual CSimulatedAircraft &ownAircraft() = 0;

            // cockpit related updates, which can come from multiple sources
            // and are subject of roundtrips

            //! Update cockpit, but send signals when applicable
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) = 0;

            //! Update cockpit, but send signals when applicable
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator) = 0;

            //! Update cockpit, but send signals when applicable
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const QString &originator) = 0;
        };

        //! For testing
        class COwnAircraftProviderDummy : public QObject, public IOwnAircraftProvider
        {
            Q_OBJECT

        public:
            //! Constructor
            COwnAircraftProviderDummy() = default;

            //! Singleton
            static COwnAircraftProviderDummy *instance();

            //! \copydoc IOwnAircraftProviderReadOnly::ownAircraft
            virtual const CSimulatedAircraft &ownAircraft() const { return this->m_ownAircraft; }

            //! \copydoc IOwnAircraftProvider::ownAircraft
            virtual CSimulatedAircraft &ownAircraft() { return this->m_ownAircraft; }

            //! \copydoc IOwnAircraftProvider::getOwnAircraft
            virtual CSimulatedAircraft getOwnAircraft() const { return this->m_ownAircraft; }

        public slots:
            //! \copydoc IOwnAircraftProvider::updateCockpit
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) override;

            //! \copydoc IOwnAircraftProvider::updateComFrequency
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator) override;

            //! \copydoc IOwnAircraftProvider::updateSelcal
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const QString &originator) override;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft;
        };

        //! Class which can be directly used providing access to such an interface
        class COwnAircraftProviderSupportReadOnly
        {
        public:
            //! \copydoc IOwnAircraftProviderReadOnly::ownAircraft
            virtual const CSimulatedAircraft &ownAircraft() const;

        protected:
            //! Constructor
            COwnAircraftProviderSupportReadOnly(const IOwnAircraftProviderReadOnly *ownAircraftProvider) : m_ownAircraftProvider(ownAircraftProvider) {}
            const IOwnAircraftProviderReadOnly *m_ownAircraftProvider = nullptr; //!< access to object
        };

        //! Delegating class which can be directly used providing access to such an interface
        class COwnAircraftProviderSupport
        {
        public:
            //! \copydoc IOwnAircraftProviderReadOnly::ownAircraft
            virtual const CSimulatedAircraft &ownAircraft() const;

            //! \copydoc IOwnAircraftProvider::ownAircraft
            virtual CSimulatedAircraft &ownAircraft();

            //! \copydoc IOwnAircraftProvider::updateCockpit
            virtual bool providerUpdateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator);

            //! \copydoc IOwnAircraftProvider::updateComFrequency
            virtual bool providerUpdateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator);

            //! \copydoc IOwnAircraftProvider::updateSelcal
            virtual bool providerUpdateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const QString &originator);

        protected:
            //! Constructor
            COwnAircraftProviderSupport(IOwnAircraftProvider *ownAircraftProvider) : m_ownAircraftProvider(ownAircraftProvider) { Q_ASSERT(ownAircraftProvider); }
            IOwnAircraftProvider *m_ownAircraftProvider = nullptr; //!< access to object
        };

    } // namespace
} // namespace

#endif // guard
