/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

};

} // namespace
} // namespace

#endif // guard
