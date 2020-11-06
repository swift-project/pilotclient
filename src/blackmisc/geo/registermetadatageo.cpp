/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadatageo.h"
#include "geo.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Geo
    {
        void registerMetadata()
        {
            CCoordinateGeodetic::registerMetadata();
            CCoordinateGeodeticList::registerMetadata();
            CLatitude::registerMetadata();
            CLongitude::registerMetadata();
            CElevationPlane::registerMetadata();
        }
    }

#if defined(Q_OS_WIN) && defined(Q_CC_CLANG)
    namespace Private
    {
        template void maybeRegisterMetaListConvert<Geo::CCoordinateGeodeticList>(int);
        template void maybeRegisterMetaListConvert<CSequence<Geo::CCoordinateGeodetic>>(int);
    } // ns
#endif

} // ns
