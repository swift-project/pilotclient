/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/pq/angle.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/math/mathutils.h"

#include <cmath>

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        CAngle::CAngle(int degrees, int minutes, double seconds) :
            CPhysicalQuantity(
                degrees + minutes / 100.0 + seconds / 10000.0,
                CAngleUnit::sexagesimalDeg())
        {
            Q_ASSERT_X((degrees >= 0 && minutes >= 0 && seconds >= 0) ||
                       (degrees <= 0 && minutes <= 0 && seconds <= 0), Q_FUNC_INFO, "Same sign required");
        }

        CAngle::CAngle(int degrees, double minutes) :
            CPhysicalQuantity(
                degrees + minutes / 100.0,
                CAngleUnit::sexagesimalDeg())
        {
            Q_ASSERT_X((degrees >= 0 && minutes >= 0) || (degrees <= 0 && minutes <= 0),
                       Q_FUNC_INFO, "Same sign required");
        }

        void CAngle::unifySign(int degrees, int &minutes, double &seconds)
        {
            minutes = std::copysign(minutes, degrees == 0 ? minutes : degrees);
            seconds = std::copysign(seconds, degrees == 0 ? minutes : degrees);
        }

        void CAngle::unifySign(int degrees, int &minutes)
        {
            if (degrees == 0) { return; }
            minutes = std::copysign(minutes, degrees);
        }

        CIcons::IconIndex CAngle::toIcon() const
        {
            return CIcons::StandardIconArrowMediumNorth16;
        }

        CAngle::DegMinSecFractionalSec CAngle::asSexagesimalDegMinSec(bool range180Degrees) const
        {
            double dms = this->value(CAngleUnit::sexagesimalDeg());

            if (range180Degrees)
            {
                dms = std::fmod(dms + 180.0, 360.0);
                dms += (dms < 0) ? 180.0 : -180.0;
            }

            DegMinSecFractionalSec values;
            if (dms < 0)
            {
                values.sign = -1;
                dms *= -1.0;
            }

            QString str = QStringLiteral("%1").arg(dms, 14, 'f', 10, '0'); // 000.0000000000
            values.deg = str.midRef(0, 3).toInt();
            values.min = str.midRef(4, 2).toInt();
            values.sec = str.midRef(6, 2).toInt();
            values.fractionalSec = str.midRef(8, 6).toInt() / 1000000.0;
            return values;
        }

        double CAngle::piFactor() const
        {
            return Math::CMathUtils::round(this->value(CAngleUnit::rad()) / CMathUtils::PI(), 6);
        }

        const double &CAngle::PI()
        {
            return CMathUtils::PI();
        }

        double CAngle::sin() const
        {
            return std::sin(this->value(CAngleUnit::rad()));
        }

        double CAngle::cos() const
        {
            return std::cos(this->value(CAngleUnit::rad()));
        }

        double CAngle::tan() const
        {
            return std::tan(this->value(CAngleUnit::rad()));
        }

        void CAngle::normalizeToPlusMinus180Degrees()
        {
            const double v = normalizeDegrees180(this->value(CAngleUnit::deg()));
            const CAngleUnit u = this->getUnit();
            *this = CAngle(v, CAngleUnit::deg());
            this->switchUnit(u);
        }

        void CAngle::normalizeTo360Degrees()
        {
            const double v = normalizeDegrees360(this->value(CAngleUnit::deg()));
            const CAngleUnit u = this->getUnit();
            *this = CAngle(v, CAngleUnit::deg());
            this->switchUnit(u);
        }

        CAngle CAngle::normalizedToPlusMinus180Degrees() const
        {
            CAngle copy(*this);
            copy.normalizeToPlusMinus180Degrees();
            return copy;
        }

        CAngle CAngle::normalizedTo360Degrees() const
        {
            CAngle copy(*this);
            copy.normalizeTo360Degrees();
            return copy;
        }

        double CAngle::normalizeDegrees180(double degrees, int roundDigits)
        {
            double d = CMathUtils::normalizeDegrees360(degrees + 180.0) - 180.0;
            if (d <= -180.0) { d = 180.0; } // -180 -> 180
            return roundDigits < 0 ? d : CMathUtils::round(d, roundDigits);
        }

        double CAngle::normalizeDegrees360(double degrees, int roundDigits)
        {
            const double d = CMathUtils::normalizeDegrees360(degrees);
            return roundDigits < 0 ? d : CMathUtils::round(d, roundDigits);
        }
    } // ns
} // ns
