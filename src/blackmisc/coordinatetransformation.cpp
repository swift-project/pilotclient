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

    bool inverse;
    invDcm.setZero();
    invDcm = dcm.inverse(inverse);
    Q_ASSERT_X(inverse, "toEcef", "Inverse matrix could not be calculated");

    CVector3D tempResult = invDcm * ned.toMathVector(); // to generic vector
    CCoordinateEcef result(tempResult);
    return result;
}

/*
 * Geodetic to ECEF
 */
CCoordinateEcef CCoordinateTransformation::toEcef(const CCoordinateGeodetic &geo)
{
    // TODO: Clarify the comparisons with fixed angles (==90, ==180) -> what happens here

    CLatitude lat = geo.latitude();
    CLongitude lon = geo.longitude();
    double latDeg = lat.value(CAngleUnit::deg());
    double lonDeg = lon.value(CAngleUnit::deg());

    double phi = lat.value(CAngleUnit::rad());
    double lambdaRad = lon.value(CAngleUnit::rad());
    double sphi = sin(phi);
    double cphi = 0;
    if (abs(latDeg) != 90) cphi = cos(phi);

    double n = EarthRadiusMeters() / sqrt(1 - e2() * CMath::square(sphi));

    double slambda = 0;
    if (lonDeg != -180) slambda = sin(lambdaRad);

    double clambda = 0;
    if (abs(lonDeg) != 90) clambda = cos(lambdaRad);

    double h = geo.height().convertedSiValueToDouble();
    double X = (n + h) * cphi;
    double Y = X * slambda;
    X *= clambda;
    double Z = (e2m() * n + h) * sphi;
    CCoordinateEcef result(X, Y, Z);
    return result;
}

/*
 * Convert to NED
 */
CCoordinateNed CCoordinateTransformation::toNed(const CCoordinateEcef &ecef, const CCoordinateGeodetic &referencePosition)
{

    CLatitude lat = referencePosition.latitude();
    CLongitude lon = referencePosition.longitude();
    double angleRad = - (lat.value(CAngleUnit::rad())) - BlackMisc::Math::PI / 2;

    CMatrix3x3 dcm1;
    CMatrix3x3 dcm2(0.0);
    CMatrix3x3 dcm3(0.0);
    CMatrix3x3 dcm(0.0);
    dcm1.setToIdentity();

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

    CVector3D tempResult = dcm * ecef.toMathVector(); // to generic vector
    CCoordinateNed result(referencePosition, tempResult);
    return result;
}

/*
 * ECEF to geodetic
 */
CCoordinateGeodetic CCoordinateTransformation::toGeodetic(const CCoordinateEcef &ecef)
{
    double R = CMath::hypot(ecef.x(), ecef.y());
    double slam = 0;
    double clam = 1;

    if (R != 0)
    {
        slam = ecef.y() / R;
        clam = ecef.x() / R;
    }

    // Calculate the distance to the earth
    double h = CMath::hypot(R, ecef.z());

    double sphi = 0;
    double cphi = 0;

    double p = CMath::square(R / EarthRadiusMeters());
    double q = e2m() * CMath::square(ecef.z() / EarthRadiusMeters());
    double r = (p + q - e4()) / 6.0;

    if (!(e4() *q == 0 && r <= 0))
    {
        // Avoid possible division by zero when r = 0 by multiplying
        // equations for s and t by r^3 and r, resp.

        double S = e4() * p * q / 4; //! S = r^3 * s
        double r2 = CMath::square(r);
        double r3 = r * r2;
        double disc = S * (2 * r3 + S);
        double u = r;

        if (disc >= 0)
        {
            double T3 = S + r3;
            /*
              Pick the sign on the sqrt to maximize abs(T3). This minimizes
              loss of precision due to cancellation. The result is unchanged
              because of the way the T is used in definition of u.
            */
            T3 += T3 < 0 ? -sqrt(disc) : sqrt(disc); // T3 = (r * t)^3

            //!N.B. cubicRootReal always returns the real root. cubicRootReal(-8) = -2.
            double T = CMath::cubicRootReal(T3);

            // T can be zero; but then r2 / T -> 0.
            u += T + (T != 0 ? r2 / T : 0);
        }
        else
        {
            // T is complex, but the way u is defined the result is real.
            double ang = atan2(sqrt(-disc), -(S + r3));
            /*
              There are three possible cube roots. We choose the root which
              avoids cancellation. Note that disc < 0 implies that r < 0.
            */
            u += 2 * r * cos(ang / 3);
        }

        // This is garanteed positive
        double V = sqrt(CMath::square(u) + e4() * q);

        /*
          Avoid loss of accuracy when u < 0. Underflow doesn't occur in
          e4 * q / (v - u) because u ~ e^4 when q is small and u < 0.
        */
        double uv = u < 0 ? e4() * q / (V - u) : u + V; //! u+v, guaranteed positive

        // Need to guard against w going negative due to roundoff in uv - q.
        double w = std::max(double(0), e2abs() * (uv - q) / (2 * V));

        /*
          Rearrange expression for k to avoid loss of accuracy due to
          subtraction. Division by 0 not possible because uv > 0, w >= 0.
        */
        double k = uv / (sqrt(uv + CMath::square(w)) + w);
        double k1 = k;
        double k2 = k + e2();
        double d = k1 * R / k2;
        double H = CMath::hypot((ecef.z()) / k1, R / k2);

        sphi = (ecef.z() / k1) / H;
        cphi = (R / k2) / H;

        h = (1 - e2m() / k1) * CMath::hypot(d, ecef.z());
    }
    else // e4 * q == 0 && r <= 0
    {
        /*
          This leads to k = 0 (oblate, equatorial plane) and k + e^2 = 0
          (prolate, rotation axis) and the generation of 0/0 in the general
          formulas for phi and h. using the general formula and division by 0
          in formula for h. So handle this case by taking the limits:
          f > 0: z -> 0, k -> e2 * sqrt(q)/sqrt(e4 - p)
          f < 0: R -> 0, k + e2 -> - e2 * sqrt(q)/sqrt(e4 - p)
        */
        double zz = sqrt((e4() - p) / e2m());
        double xx = sqrt(p);
        double H = CMath::hypot(zz, xx);
        sphi = zz / H;
        cphi = xx / H;
        if (ecef.z() < 0) sphi = -sphi; // for tiny negative Z (not for prolate)
        h = - EarthRadiusMeters() * (e2m()) * H / e2abs();
    }

    double latRad = atan2(sphi, cphi);
    double lonRad = -atan2(-slam, clam); // Negative signs return lon in [-180, 180)
    CCoordinateGeodetic result(
        CLatitude(latRad, CAngleUnit::rad()),
        CLongitude(lonRad, CAngleUnit::rad()),
        CLength(h, CLengthUnit::m()));
    result.switchUnit(CAngleUnit::deg());
    return result;
}

} // namespace
} // namespace
