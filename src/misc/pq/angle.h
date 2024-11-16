// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_ANGLE_H
#define SWIFT_MISC_PQ_ANGLE_H

#include <QMetaType>
#include <QPair>
#include <QString>

#include "misc/math/mathutils.h"
#include "misc/mixin/mixinicon.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    //! Physical unit angle (radians, degrees)
    class SWIFT_MISC_EXPORT CAngle : public CPhysicalQuantity<CAngleUnit, CAngle>
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
            QString fractionalSecAsString(int width = -1) const
            {
                return swift::misc::math::CMathUtils::fractionalPartAsString(fractionalSec, width);
            }
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

        //! \copydoc swift::misc::mixin::Icon::toIcon
        swift::misc::CIcons::IconIndex toIcon() const;

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
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CAngle)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CAnglePair)

#endif // guard
