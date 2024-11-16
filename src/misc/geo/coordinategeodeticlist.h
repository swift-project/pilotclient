// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_GEO_COORDINATEGEODETICLIST_H
#define SWIFT_MISC_GEO_COORDINATEGEODETICLIST_H

#include <QMetaType>

#include "misc/collection.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/elevationplane.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/pq/length.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::geo, CCoordinateGeodetic, CCoordinateGeodeticList)

namespace swift::misc::geo
{
    //! Value object encapsulating a list of coordinates.
    class SWIFT_MISC_EXPORT CCoordinateGeodeticList :
        public CSequence<CCoordinateGeodetic>,
        public IGeoObjectList<CCoordinateGeodetic, CCoordinateGeodeticList>,
        public mixin::MetaType<CCoordinateGeodeticList>,
        public mixin::JsonOperators<CCoordinateGeodeticList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CCoordinateGeodeticList)
        using CSequence::CSequence;

        //! Default constructor.
        CCoordinateGeodeticList();

        //! Construct from a base class object.
        CCoordinateGeodeticList(const CSequence<CCoordinateGeodetic> &other);

        //! Average height within range and having an height
        CElevationPlane averageGeodeticHeight(const CCoordinateGeodetic &reference, const physical_quantities::CLength &range, const physical_quantities::CLength &maxDeviation = physical_quantities::CLength(1.0, physical_quantities::CLengthUnit::m()), int minValues = 3, int sufficentValues = 5) const;
    };
} // namespace swift::misc::geo

Q_DECLARE_METATYPE(swift::misc::geo::CCoordinateGeodeticList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::geo::CCoordinateGeodetic>)

#endif
