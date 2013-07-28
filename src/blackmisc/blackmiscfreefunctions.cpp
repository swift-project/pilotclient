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
    CNavSystem::registerMetadata();
    CAdfSystem::registerMetadata();
    CAltitude::registerMetadata();
    CAviationVerticalPositions::registerMetadata();
    CTransponder::registerMetadata();
    CHeading::registerMetadata();
    CTrack::registerMetadata();
}

/*
 * Metadata for Math
 */
void BlackMisc::Math::registerMetadata()
{
    CMatrix3x3::registerMetadata();
    CMatrix3x1::registerMetadata();
    CMatrix1x3::registerMetadata();
    CVector3D::registerMetadata();
}


/*
 * Metadata for Geo
 */
void BlackMisc::Geo::registerMetadata()
{
    CCoordinateEcef::registerMetadata();
    CCoordinateNed::registerMetadata();
    CCoordinateGeodetic::registerMetadata();
    CLatitude::registerMetadata();
    CLongitude::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
}

/*
 * Init resources
 */
void BlackMisc::initResources()
{
    initBlackMiscResources();
}
