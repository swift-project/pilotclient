/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * Copyright (c) Charles Karney (2008-2011) <charles@karney.com> and licensed
 * under the MIT/X11 License. For more information, see
 * http://geographiclib.sourceforge.net/
*/

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

/*!
 * \brief Coordinate transformation class between different systems
 */
class CCoordinateTransformation
{
private:
    /*!
     * \brief Equatorial radius of WGS84 ellipsoid (6378137 m)
     * \return
     */
    static const qreal &EarthRadiusMeters()
    {
        static qreal erm = 6378137.0;
        return erm;
    }

    /*!
     * \brief Flattening of WGS84 ellipsoid (1/298.257223563)
     * \return
     */
    static const qreal &Flattening()
    {
        static qreal f = 1/298.257223563;
        return f;
    }

    /*!
     * \brief First eccentricity squared
     * \return
     */
    static const qreal &e2()
    {
        static qreal e2 = (Flattening() * (2 - Flattening()));
        return e2;
    }

    /*!
     * \brief First eccentricity to power of four
     * \return
     */
    static const qreal &e4()
    {
        static qreal e4 = BlackMisc::Math::CMath::square(e2());
        return e4;
    }

    /*!
     * \brief First eccentricity squared absolute
     * \return
     */
    static const qreal &e2abs()
    {
        static qreal e2abs = abs(e2());
        return e2abs;
    }

    /*!
     * \brief Eccentricity e2m
     * \return
     */
    static const qreal &e2m()
    {
        static qreal e2m = BlackMisc::Math::CMath::square(1 - Flattening());
        return e2m;
    }

    /*!
     * \brief Default constructor, avoid object instantiation
     */
    CCoordinateTransformation() {}


public:
    /*!
     * \brief NED to ECEF
     * \param ned
     * \return
     */
    static CCoordinateEcef toEcef(const CCoordinateNed &ned);

    /*!
     * \brief ECEF via Geodetic to NED
     * \param geo
     * \return
     */
    static CCoordinateNed toNed(const CCoordinateEcef &ecef, const CCoordinateGeodetic &geo);

    /*!
     * \brief ECEF to Geodetic
     * \param geo
     * \return
     */
    static CCoordinateGeodetic toGeodetic(const CCoordinateEcef &ecef);

};

} // namespace
} // namespace

#endif // guard
