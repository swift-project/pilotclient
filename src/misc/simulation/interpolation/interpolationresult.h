//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONRESULT_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONRESULT_H

#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/interpolation/interpolationstatus.h"
#include "misc/simulation/partsstatus.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Combined results
    class SWIFT_MISC_EXPORT CInterpolationResult
    {
    public:
        //! Ctor
        CInterpolationResult() = default;

        //! Get situation
        const aviation::CAircraftSituation &getInterpolatedSituation() const { return m_interpolatedSituation; }

        //! Get parts (interpolated or guessed)
        const aviation::CAircraftParts &getInterpolatedParts() const { return m_interpolatedParts; }

        //! Get status
        const CInterpolationStatus &getInterpolationStatus() const { return m_interpolationStatus; }

        //! Get status
        const CPartsStatus &getPartsStatus() const { return m_partsStatus; }

        //! Set situation
        void setInterpolatedSituation(const aviation::CAircraftSituation &situation)
        {
            m_interpolatedSituation = situation;
        }

        //! Set parts (interpolated or guessed)
        void setInterpolatedParts(const aviation::CAircraftParts &parts) { m_interpolatedParts = parts; }

        //! Set values
        void setValues(const aviation::CAircraftSituation &situation, const aviation::CAircraftParts &parts);

        //! Set status
        void setInterpolationStatus(const CInterpolationStatus &status) { m_interpolationStatus = status; }

        //! Set status
        void setPartsStatus(const CPartsStatus &status) { m_partsStatus = status; }

        //! Set status values
        void setStatus(const CInterpolationStatus &interpolation, const CPartsStatus &parts);

        //! @{
        //! Implicit conversion
        operator const aviation::CAircraftSituation &() const { return m_interpolatedSituation; }
        operator const aviation::CAircraftParts &() const { return m_interpolatedParts; }
        //! @}

    private:
        aviation::CAircraftSituation m_interpolatedSituation =
            aviation::CAircraftSituation::null(); //!< interpolated situation
        aviation::CAircraftParts m_interpolatedParts =
            aviation::CAircraftParts::null(); //!< guessed or interpolated parts
        CInterpolationStatus m_interpolationStatus; //!< interpolation status
        CPartsStatus m_partsStatus; //!< parts status
    };

} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATIONRESULT_H
