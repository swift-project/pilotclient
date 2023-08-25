// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_GEO_COORDINATEGEODETICLIST_H
#define BLACKMISC_GEO_COORDINATEGEODETICLIST_H

#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/geoobjectlist.h"

#include "blackmisc/pq/length.h"

#include "blackmisc/mixin/mixinmetatype.h"
#include "blackmisc/mixin/mixinjson.h"

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Geo, CCoordinateGeodetic, CCoordinateGeodeticList)

namespace BlackMisc::Geo
{
    //! Value object encapsulating a list of coordinates.
    class BLACKMISC_EXPORT CCoordinateGeodeticList :
        public CSequence<CCoordinateGeodetic>,
        public IGeoObjectList<CCoordinateGeodetic, CCoordinateGeodeticList>,
        public Mixin::MetaType<CCoordinateGeodeticList>,
        public Mixin::JsonOperators<CCoordinateGeodeticList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CCoordinateGeodeticList)
        using CSequence::CSequence;

        //! Default constructor.
        CCoordinateGeodeticList();

        //! Construct from a base class object.
        CCoordinateGeodeticList(const CSequence<CCoordinateGeodetic> &other);

        //! Average height within range and having an height
        CElevationPlane averageGeodeticHeight(const CCoordinateGeodetic &reference, const PhysicalQuantities::CLength &range, const PhysicalQuantities::CLength &maxDeviation = PhysicalQuantities::CLength(1.0, PhysicalQuantities::CLengthUnit::m()), int minValues = 3, int sufficentValues = 5) const;
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodeticList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Geo::CCoordinateGeodetic>)

#endif // guard
