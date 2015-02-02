/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMDIRECTACCESSRENDEREDAIRCRAFT_H
#define BLACKMISC_SIMDIRECTACCESSRENDEREDAIRCRAFT_H

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/avaircraftsituationlist.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to rendered aircraft
        class IRenderedAircraftProviderReadOnly
        {
        public:
            //! All rendered aircraft
            virtual const CSimulatedAircraftList &renderedAircraft() const = 0;

            //! All rendered aircraft
            virtual const BlackMisc::Aviation::CAircraftSituationList &renderedAircraftSituations() const = 0;

            //! Destructor
            virtual ~IRenderedAircraftProviderReadOnly() {}
        };

        //! Direct in memory access to own aircraft
        class IRenderedAircraftProvider : public IRenderedAircraftProviderReadOnly
        {
        public:
            //! All rendered aircraft
            virtual CSimulatedAircraftList &renderedAircraft() = 0;

            //! Rendered aircraft situations (history)
            virtual BlackMisc::Aviation::CAircraftSituationList &renderedAircraftSituations() = 0;

            //! Enable/disable rendering
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator) = 0;

            //! Change model string
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) = 0;
        };

        //! Class which can be directly used providing access to such an interface
        class CRenderedAircraftProviderSupportReadOnly
        {
        public:
            //! \copydoc IRenderedAircraftProviderReadOnly::renderedAircraft
            virtual const CSimulatedAircraftList &renderedAircraft() const;

            //!\copydoc IRenderedAircraftProviderReadOnly::renderedAircraftSituations
            virtual const BlackMisc::Aviation::CAircraftSituationList &renderedAircraftSituations() const = 0;

        protected:
            //! Constructor
            CRenderedAircraftProviderSupportReadOnly(const IRenderedAircraftProviderReadOnly *renderedAircraftProvider) : m_renderedAircraftProvider(renderedAircraftProvider) {}
            const IRenderedAircraftProviderReadOnly *m_renderedAircraftProvider = nullptr; //!< access to object
        };

        //! Delegating class which can be directly used providing access to such an interface
        class CRenderedAircraftProviderSupport
        {
        public:
            //! \copydoc IRenderedAircraftProviderReadOnly::renderedAircraft
            virtual const CSimulatedAircraftList &renderedAircraft() const;

            //! \copydoc IRenderedAircraftProvider::renderedAircraft
            virtual CSimulatedAircraftList &renderedAircraft();

            //!\copydoc IRenderedAircraftProviderReadOnly::renderedAircraftSituations
            virtual const BlackMisc::Aviation::CAircraftSituationList &renderedAircraftSituations() const;

            //!\copydoc IRenderedAircraftProvider::renderedAircraftSituations
            virtual Aviation::CAircraftSituationList &renderedAircraftSituations();

            //! \copydoc IRenderedAircraftProvider::updateAircraftEnabled
            virtual bool providerUpdateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator);

            //! \copydoc IRenderedAircraftProvider::updateAircraftModel
            virtual bool providerUpdateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator);

        protected:
            //! Constructor
            CRenderedAircraftProviderSupport(IRenderedAircraftProvider *renderedAircraftProvider) : m_renderedAircraftProvider(renderedAircraftProvider) { Q_ASSERT(renderedAircraftProvider); }
            IRenderedAircraftProvider *m_renderedAircraftProvider = nullptr; //!< access to object
        };

    } // namespace
} // namespace

#endif // guard
