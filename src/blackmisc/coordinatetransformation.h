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

};

} // namespace
} // namespace

#endif // guard
