//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolationresult.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc::Simulation
{
    void CInterpolationResult::setValues(const CAircraftSituation &situation, const CAircraftParts &parts)
    {
        m_interpolatedSituation = situation;
        m_interpolatedParts = parts;
    }

    void CInterpolationResult::setStatus(const CInterpolationStatus &interpolation, const CPartsStatus &parts)
    {
        m_interpolationStatus = interpolation;
        m_partsStatus = parts;
    }
}