// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/geo/coordinategeodeticlist.h"

#include "misc/math/mathutils.h"

using namespace swift::misc::math;
using namespace swift::misc::physical_quantities;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::geo, CCoordinateGeodetic, CCoordinateGeodeticList)

namespace swift::misc::geo
{
    CCoordinateGeodeticList::CCoordinateGeodeticList(const CSequence<CCoordinateGeodetic> &other)
        : CSequence<CCoordinateGeodetic>(other)
    {}

    CElevationPlane CCoordinateGeodeticList::averageGeodeticHeight(const CCoordinateGeodetic &reference,
                                                                   const CLength &range, const CLength &maxDeviation,
                                                                   int minValues, int sufficentValues) const
    {
        if (this->size() < minValues) { return CElevationPlane::null(); } // no chance to succeed

        QList<double> valuesInFt;
        const CCoordinateGeodeticList sorted = this->findWithGeodeticMSLHeight()
                                                   .findWithinRange(reference, range)
                                                   .sortedByEuclideanDistanceSquared(reference);
        if (sorted.size() < minValues) { return CElevationPlane::null(); }

        // we know all values have MSL and are within range
        for (const CCoordinateGeodetic &coordinate : sorted)
        {
            const double elvFt = coordinate.geodeticHeight().value(CLengthUnit::ft());
            valuesInFt.push_back(elvFt);
            if (valuesInFt.size() >= sufficentValues) { break; }
        }

        if (valuesInFt.size() < minValues) { return CElevationPlane::null(); }

        const double MaxDevFt = maxDeviation.value(CLengthUnit::ft());
        const QPair<double, double> elvStdDevMean = CMathUtils::standardDeviationAndMean(valuesInFt);
        if (elvStdDevMean.first > MaxDevFt) { return CElevationPlane::null(); }
        return { reference, elvStdDevMean.second, CElevationPlane::singlePointRadius() };
    }

} // namespace swift::misc::geo
