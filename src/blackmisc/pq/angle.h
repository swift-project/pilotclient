/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_ANGLE_H
#define BLACKMISC_PQ_ANGLE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"

#include <QMetaType>
#include <QString>
#include <QPair>

namespace BlackMisc::PhysicalQuantities
{
    //! Physical unit angle (radians, degrees)
    class BLACKMISC_EXPORT CAngle : public CPhysicalQuantity<CAngleUnit, CAngle>
    {
    public:
        //! Default constructor
        CAngle() : CPhysicalQuantity(0, CAngleUnit::defaultUnit()) {}

        //! Init by double value
        CAngle(double value, const CAngleUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CAngle(const QString &unitString) : CPhysicalQuantity(unitString) {}

        //! Value as individual values
        struct DegMinSecFractionalSec
        {
            int sign = 1; //!< 1/-1
            int deg = 0; //!< 0-359
            int min = 0; //!< 0-59
            int sec = 0; //!< 0-59
            double fractionalSec = 0; //!< value < 1.0

            //! Degrees as string
            QString degAsString() const { return QString::number(deg); }

            //! Minutes as string
            QString minAsString() const { return QString::number(min); }

            //! Seconds as string
            QString secAsString() const { return QString::number(sec); }

            //! Fractional seconds as string
            QString fractionalSecAsString(int width = -1) const { return BlackMisc::Math::CMathUtils::fractionalPartAsString(fractionalSec, width); }
        };

        //! \brief Init as sexagesimal degrees, minutes, seconds
        //! The sign of all parameters must be the same, either all positive or all negative.
        //! \see CAngle::unifySign(int &, int &, double &)
        CAngle(int degrees, int minutes, double seconds);

        //! \brief Init as sexagesimal degrees, minutes
        //! The sign of both parameters must be the same, either both positive or both negative.
        //! \see CAngle::unifySign(int &, double &)
        CAngle(int degrees, double minutes);

        //! Minutes and secods will get same sign as degrees
        static void unifySign(int degrees, int &minutes, double &seconds);

        //! Minutes will get same sign as degrees
        static void unifySign(int degrees, int &minutes);

        //! \copydoc BlackMisc::Mixin::Icon::toIcon
        BlackMisc::CIcons::IconIndex toIcon() const;

        //! As individual values
        DegMinSecFractionalSec asSexagesimalDegMinSec(bool range180Degrees = false) const;

        //! Value as factor of PI (e.g. 0.5PI)
        double piFactor() const;

        //! PI as convenience method
        static const double &PI();

        //! Sine of angle
        double sin() const;

        //! Cosine of angle
        double cos() const;

        //! Tangent of angle
        double tan() const;

        //! Normalize to +- 180deg, [-179.99, 180.0]
        void normalizeToPlusMinus180Degrees();

        //! Normalize to 0-360, [0, 359,99]
        void normalizeTo360Degrees();

        //! As [-179.99, 180.0] normalized angle
        CAngle normalizedToPlusMinus180Degrees() const;

        //! As [0, 359.99] normalized angle
        CAngle normalizedTo360Degrees() const;

        //! Normalize: -180< degrees ≤180
        static double normalizeDegrees180(double degrees, int roundDigits = -1);

        //! Normalize: 0≤ degrees <360
        static double normalizeDegrees360(double degrees, int roundDigits = -1);
    };

    using CAnglePair = QPair<CAngle, CAngle>; //!< Pair of angle
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAngle)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAnglePair)

#endif // guard
