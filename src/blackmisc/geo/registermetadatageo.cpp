/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/geo/registermetadatageo.h"

// Geo headers
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/coordinategeodeticlist.h"
#include "blackmisc/geo/elevationplane.h"

namespace BlackMisc::Geo
{
    void registerMetadata()
    {
        CCoordinateGeodetic::registerMetadata();
        CCoordinateGeodeticList::registerMetadata();
        CLatitude::registerMetadata();
        CLongitude::registerMetadata();
        CElevationPlane::registerMetadata();
    }

} // ns
