/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadatapq.h"
#include "pq.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        void registerMetadata()
        {
            CAcceleration::registerMetadata();
            CAccelerationUnit::registerMetadata();
            CAngle::registerMetadata();
            CAngleUnit::registerMetadata();
            CFrequency::registerMetadata();
            CFrequencyUnit::registerMetadata();
            CLength::registerMetadata();
            CLengthUnit::registerMetadata();
            CMass::registerMetadata();
            CMassUnit::registerMetadata();
            CPqString::registerMetadata();
            CPressure::registerMetadata();
            CPressureUnit::registerMetadata();
            CSpeed::registerMetadata();
            CSpeedUnit::registerMetadata();
            CTemperature::registerMetadata();
            CTemperatureUnit::registerMetadata();
            CTime::registerMetadata();
            CTimeUnit::registerMetadata();
        }
    } // ns
} // ns
