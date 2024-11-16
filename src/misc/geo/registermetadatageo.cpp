// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/geo/registermetadatageo.h"

// Geo headers
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/coordinategeodeticlist.h"
#include "misc/geo/elevationplane.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"

namespace swift::misc::geo
{
    void registerMetadata()
    {
        CCoordinateGeodetic::registerMetadata();
        CCoordinateGeodeticList::registerMetadata();
        CLatitude::registerMetadata();
        CLongitude::registerMetadata();
        CElevationPlane::registerMetadata();
    }

} // namespace swift::misc::geo
