/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_INTERPOLATOR_H
#define BLACKCORE_INTERPOLATOR_H

#include "blackmisc/avaircraftsituation.h"
#include "simulator.h"

namespace BlackCore
{
    //! Interpolator, calculation inbetween positions
    class IInterpolator
    {
    public:

        //! Virtual destructor
        virtual ~IInterpolator() {}

        //! Has situations?
        virtual bool hasEnoughAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            Q_ASSERT(m_provider);
            //! \todo Interpolator, it would be more efficient to directly getting the values and decide then
            return m_provider->renderedAircraftSituations().findBeforeNowMinusOffset(6000).findByCallsign(callsign).size() > 1;
        }

        //! Current interpolated situation
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentInterpolatedSituation(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

    protected:
        //! Constructor
        IInterpolator(BlackMisc::Simulation::IRenderedAircraftProviderReadOnly *provider) : m_provider(provider) { Q_ASSERT(provider);}
        BlackMisc::Simulation::IRenderedAircraftProviderReadOnly *m_provider = nullptr; //!< access to provider
    };

} // namespace

#endif // guard
