// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
