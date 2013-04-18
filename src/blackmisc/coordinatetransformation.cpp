/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
    lat.switchUnit(CAngleUnit::rad());
    CLongitude lon = ned.referencePosition().longitude();
    lon.switchUnit(CAngleUnit::rad());

    double angleRad = - (lat.unitValueToDouble()) - BlackMisc::Math::PI / 2;
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

    angleRad = lon.unitValueToDouble();

    dcm3(0, 0) = cos(angleRad);
    dcm3(0, 1) = sin(angleRad);
    dcm3(1, 0) = -sin(angleRad);
    dcm3(1, 1) = cos(angleRad);
    dcm3(2, 2) = 1;

    dcm = dcm1 * dcm2 * dcm3;

    invDcm.setZero();
    invDcm = dcm.inverse();

    CCoordinateNed tempResult(ned);
    tempResult.matrixMultiplication(invDcm);
    CCoordinateEcef result(tempResult.north(), tempResult.east(), tempResult.down());

    return result;

}

} // namespace
} // namespace
