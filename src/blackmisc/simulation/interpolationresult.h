//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONRESULT_H
#define BLACKMISC_SIMULATION_INTERPOLATIONRESULT_H

#include "blackmisc/blackmiscexport.h"

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftparts.h"

#include "blackmisc/simulation/interpolationstatus.h"
#include "blackmisc/simulation/partsstatus.h"

namespace BlackMisc::Simulation
{
    //! Combined results
    class BLACKMISC_EXPORT CInterpolationResult
    {
    public:
        //! Ctor
        CInterpolationResult() = default;

        //! Get situation
        const Aviation::CAircraftSituation &getInterpolatedSituation() const { return m_interpolatedSituation; }

        //! Get parts (interpolated or guessed)
        const Aviation::CAircraftParts &getInterpolatedParts() const { return m_interpolatedParts; }

        //! Get status
        const CInterpolationStatus &getInterpolationStatus() const { return m_interpolationStatus; }

        //! Get status
        const CPartsStatus &getPartsStatus() const { return m_partsStatus; }

        //! Set situation
        void setInterpolatedSituation(const Aviation::CAircraftSituation &situation) { m_interpolatedSituation = situation; }

        //! Set parts (interpolated or guessed)
        void setInterpolatedParts(const Aviation::CAircraftParts &parts) { m_interpolatedParts = parts; }

        //! Set values
        void setValues(const Aviation::CAircraftSituation &situation, const Aviation::CAircraftParts &parts);

        //! Set status
        void setInterpolationStatus(const CInterpolationStatus &status) { m_interpolationStatus = status; }

        //! Set status
        void setPartsStatus(const CPartsStatus &status) { m_partsStatus = status; }

        //! Set status values
        void setStatus(const CInterpolationStatus &interpolation, const CPartsStatus &parts);

        //! @{
        //! Implicit conversion
        operator const Aviation::CAircraftSituation &() const { return m_interpolatedSituation; }
        operator const Aviation::CAircraftParts &() const { return m_interpolatedParts; }
        //! @}

    private:
        Aviation::CAircraftSituation m_interpolatedSituation = Aviation::CAircraftSituation::null(); //!< interpolated situation
        Aviation::CAircraftParts m_interpolatedParts = Aviation::CAircraftParts::null(); //!< guessed or interpolated parts
        CInterpolationStatus m_interpolationStatus; //!< interpolation status
        CPartsStatus m_partsStatus; //!< parts status
    };

}

#endif // BLACKMISC_SIMULATION_INTERPOLATIONRESULT_H
