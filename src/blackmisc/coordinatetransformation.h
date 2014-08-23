/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COORDINATETRANSFORMATION_H
#define BLACKMISC_COORDINATETRANSFORMATION_H

#include "blackmisc/coordinateecef.h"
#include "blackmisc/coordinatened.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/mathmatrix3x3.h"
#include "blackmisc/mathematics.h"

namespace BlackMisc
{
    namespace Geo
    {

        //! Coordinate transformation class between different systems
        class CCoordinateTransformation
        {

        public:
            //! NED to ECEF
            static CCoordinateEcef toEcef(const CCoordinateNed &ned);

            //! Geodetic to ECEF
            static CCoordinateEcef toEcef(const CCoordinateGeodetic &geo);

            //! ECEF via Geodetic to NED
            static CCoordinateNed toNed(const CCoordinateEcef &ecef, const CCoordinateGeodetic &referencePosition);

            //! ECEF to Geodetic
            static CCoordinateGeodetic toGeodetic(const CCoordinateEcef &ecef);

        private:
            //! Equatorial radius of WGS84 ellipsoid (6378137 m)
            static const double &EarthRadiusMeters()
            {
                static double erm = 6378137.0;
                return erm;
            }

            //! Flattening of WGS84 ellipsoid (1/298.257223563)
            static const double &Flattening()
            {
                static double f = 1.0 / 298.257223563;
                return f;
            }

            //! First eccentricity squared
            static const double &e2()
            {
                static double e2 = (Flattening() * (2 - Flattening()));
                return e2;
            }

            //! First eccentricity to power of four
            static const double &e4()
            {
                static double e4 = BlackMisc::Math::CMath::square(e2());
                return e4;
            }

            //! First eccentricity squared absolute
            static const double &e2abs()
            {
                static double e2abs = qAbs(e2());
                return e2abs;
            }

            //! Eccentricity e2m
            static const double &e2m()
            {
                static double e2m = BlackMisc::Math::CMath::square(1.0 - Flattening());
                return e2m;
            }

            //! Default constructor, deleted
            CCoordinateTransformation();

        };

    } // namespace
} // namespace

#endif // guard
