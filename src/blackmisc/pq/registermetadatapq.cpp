/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadatapq.h"
#include "blackmisc/pq/acceleration.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/mass.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

namespace BlackMisc::PhysicalQuantities
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
