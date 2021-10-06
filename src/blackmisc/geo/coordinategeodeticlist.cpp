/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/geo/coordinategeodeticlist.h"
#include "math/mathutils.h"

#include <QJsonValue>
#include <QString>
#include <QtGlobal>

using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Geo, CCoordinateGeodetic, CCoordinateGeodeticList)

namespace BlackMisc::Geo
{
    CCoordinateGeodeticList::CCoordinateGeodeticList()
    { }

    CCoordinateGeodeticList::CCoordinateGeodeticList(const CSequence<CCoordinateGeodetic> &other) :
        CSequence<CCoordinateGeodetic>(other)
    { }

    CElevationPlane CCoordinateGeodeticList::averageGeodeticHeight(const CCoordinateGeodetic &reference, const CLength &range, const CLength &maxDeviation, int minValues, int sufficentValues) const
    {
        if (this->size() < minValues) { return CElevationPlane::null(); } // no chance to succeed

        QList<double> valuesInFt;
        const CCoordinateGeodeticList sorted = this->findWithGeodeticMSLHeight().findWithinRange(reference, range).sortedByEuclideanDistanceSquared(reference);
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
        return CElevationPlane(reference, elvStdDevMean.second, CElevationPlane::singlePointRadius());
    }

} // namespace
