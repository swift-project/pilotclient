/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "pqallquantities.h"

/*
 * Metadata for PQs
 *
 * In a separate file to workaround a limitation of MinGW:
 * http://stackoverflow.com/q/16596876/1639256
 */
void BlackMisc::PhysicalQuantities::registerMetadata()
{
    CMeasurementUnit::registerMetadata();
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
    CPressure::registerMetadata();
    CPressureUnit::registerMetadata();
    CSpeed::registerMetadata();
    CSpeedUnit::registerMetadata();
    CTemperature::registerMetadata();
    CTemperatureUnit::registerMetadata();
    CTime::registerMetadata();
    CTimeUnit::registerMetadata();
    CPqString::registerMetadata();
}
