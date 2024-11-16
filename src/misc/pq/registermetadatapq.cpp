// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/pq/registermetadatapq.h"

#include "misc/pq/acceleration.h"
#include "misc/pq/angle.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/mass.h"
#include "misc/pq/pqstring.h"
#include "misc/pq/pressure.h"
#include "misc/pq/speed.h"
#include "misc/pq/temperature.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

namespace swift::misc::physical_quantities
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
} // namespace swift::misc::physical_quantities
