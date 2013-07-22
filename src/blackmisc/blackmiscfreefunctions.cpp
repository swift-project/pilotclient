/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmiscfreefunctions.h"

/*
 * Metadata for PQs
 */
void BlackMisc::PhysicalQuantities::registerMetadata()
{
    {
        CAcceleration::registerMetadata();
        CAngle::registerMetadata();
        CFrequency::registerMetadata();
        CLength::registerMetadata();
        CMass::registerMetadata();
        CPressure::registerMetadata();
        CSpeed::registerMetadata();
        CTemperature::registerMetadata();
        CTime::registerMetadata();
    }
}

/*
 * Metadata for aviation
 */
void BlackMisc::Aviation::registerMetadata()
{
    CComSystem::registerMetadata();
    CAdfSystem::registerMetadata();
    CNavSystem::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
}
