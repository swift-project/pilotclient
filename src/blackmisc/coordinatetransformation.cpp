/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
 *  Copyright (c) Charles Karney (2008-2011) <charles@karney.com> and licensed
 *  under the MIT/X11 License. For more information, see http://geographiclib.sourceforge.net/
*/

#include "coordinatetransformation.h"
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMisc
{
namespace Geo
{

/*
 * NED to ECEF
 */
CCoordinateEcef CCoordinateTransformation::toEcef(const CCoordinateNed &ned)
{
    CLatitude lat = ned.referencePosition().latitude();
    CLongitude lon = ned.referencePosition().longitude();
    double angleRad = - (lat.value(CAngleUnit::rad())) - BlackMisc::Math::PI / 2;

    CMatrix3x3 dcm1;
    CMatrix3x3 dcm2;
    CMatrix3x3 dcm3;
    CMatrix3x3 dcm;
    CMatrix3x3 invDcm;
    dcm1.setToIdentity();
    dcm2.setZero();
    dcm3.setZero();

    dcm2(0, 0) = cos(angleRad);
    dcm2(0, 2) = -sin(angleRad);
    dcm2(1, 1) = 1;
    dcm2(2, 0) = sin(angleRad);
    dcm2(2, 2) = cos(angleRad);

    angleRad = lon.value(CAngleUnit::rad());
    dcm3(0, 0) = cos(angleRad);
    dcm3(0, 1) = sin(angleRad);
    dcm3(1, 0) = -sin(angleRad);
    dcm3(1, 1) = cos(angleRad);
    dcm3(2, 2) = 1;

    dcm = dcm1 * dcm2 * dcm3;

    invDcm.setZero();
    invDcm = dcm.inverse();

    CVector3D tempResult = invDcm * ned.toMathVector(); // to generic vector
    CCoordinateEcef result(tempResult);
    return result;
}

/*
 * Convert to NED
 */
CCoordinateNed toNed(const CCoordinateEcef &ecef, const CCoordinateGeodetic &geo) {

    CLatitude lat = geo.latitude();
    CLongitude lon = geo.longitude();
    double angleRad = - (lat.value(CAngleUnit::rad())) - BlackMisc::Math::PI / 2;

    CMatrix3x3 dcm1;
    CMatrix3x3 dcm2(0.0);
    CMatrix3x3 dcm3(0.0);
    CMatrix3x3 dcm(0.0);
    dcm1.setToIdentity();

    dcm2(0,0) = cos( angleRad );
    dcm2(0,2) = -sin( angleRad );
    dcm2(1,1) = 1;
    dcm2(2,0) = sin( angleRad );
    dcm2(2,2) = cos( angleRad );

    angleRad = lon.value(CAngleUnit::rad());
    dcm3(0,0) = cos(angleRad );
    dcm3(0,1) = sin(angleRad );
    dcm3(1,0) = -sin(angleRad );
    dcm3(1,1) = cos(angleRad );
    dcm3(2,2) = 1;

    dcm = dcm1 * dcm2 * dcm3;

    CVector3D tempResult = dcm * ecef.toMathVector(); // to generic vector
    CCoordinateNed result(geo, tempResult);
    return result;
}

} // namespace
} // namespace
