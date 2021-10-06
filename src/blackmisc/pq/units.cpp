/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/pq/units.h"
#include "blackmisc/math/mathutils.h"

#include <QCoreApplication>

BLACK_DEFINE_UNIT_MIXINS(CAngleUnit)
BLACK_DEFINE_UNIT_MIXINS(CLengthUnit)
BLACK_DEFINE_UNIT_MIXINS(CPressureUnit)
BLACK_DEFINE_UNIT_MIXINS(CFrequencyUnit)
BLACK_DEFINE_UNIT_MIXINS(CMassUnit)
BLACK_DEFINE_UNIT_MIXINS(CTemperatureUnit)
BLACK_DEFINE_UNIT_MIXINS(CSpeedUnit)
BLACK_DEFINE_UNIT_MIXINS(CTimeUnit)
BLACK_DEFINE_UNIT_MIXINS(CAccelerationUnit)

namespace BlackMisc::PhysicalQuantities
{
    using BlackMisc::Math::CMathUtils;

    // pin vtables to this file

    void CLengthUnit::anchor()
    { }

    void CFrequencyUnit::anchor()
    { }

    void CMassUnit::anchor()
    { }

    void CPressureUnit::anchor()
    { }

    void CTemperatureUnit::anchor()
    { }

    void CSpeedUnit::anchor()
    { }

    void CAccelerationUnit::anchor()
    { }

    QString CAngleUnit::makeRoundedQStringWithUnit(double value, int digits, bool withGroupSeparator, bool i18n) const
    {
        if (digits < 0) { digits = this->getDisplayDigits(); }
        QString s;
        if ((*this) == CAngleUnit::sexagesimalDeg())
        {
            digits -= 4;
            Q_ASSERT(digits >= 0);
            double de = CMathUtils::trunc(value);
            double mi = CMathUtils::trunc((value - de) * 100.0);
            double se = CMathUtils::trunc((value - de - mi / 100.0) * 1000000) / 100.0;
            const char *fmt = value < 0 ? "-%1 %2 %3" : "%1 %2 %3";
            s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
            s = s.arg(fabs(de), 0, 'f', 0).arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 3 + digits, 'f', digits, '0');
        }
        else if ((*this) == CAngleUnit::sexagesimalDegMin())
        {
            digits -= 2;
            Q_ASSERT(digits >= 0);
            double de = CMathUtils::trunc(value);
            double mi = CMathUtils::trunc((value - de) * 100.0);
            const char *fmt = value < 0 ? "-%1 %2" : "%1 %2";
            s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
            s = s.arg(fabs(de), 0, 'f', 0).arg(fabs(mi), 3 + digits, 'f', digits, '0');
        }
        else
        {
            s = this->CMeasurementUnit::makeRoundedQStringWithUnit(value, digits, withGroupSeparator, i18n);
        }
        return s;
    }

    QString CTimeUnit::makeRoundedQStringWithUnit(double value, int digits, bool withGroupSeparator, bool i18n) const
    {
        if (digits < 0) { digits = this->getDisplayDigits(); }
        QString s;
        if ((*this) == CTimeUnit::hms())
        {
            digits -= 4;
            Q_ASSERT(digits >= 0);
            double hr = CMathUtils::trunc(value);
            double mi = CMathUtils::trunc((value - hr) * 100.0);
            double se = CMathUtils::trunc((value - hr - mi / 100.0) * 1000000) / 100.0;
            const char *fmt = value < 0 ? "-%1h%2m%3s" : "%1h%2m%3s";
            s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
            s = s.arg(fabs(hr), 2, 'f', 0, '0').arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 3 + digits, 'f', digits, '0');
        }
        else if ((*this) == CTimeUnit::hrmin())
        {
            digits -= 2;
            Q_ASSERT(digits >= 0);
            double hr = CMathUtils::trunc(value);
            double mi = CMathUtils::trunc((value - hr) * 100.0);
            const char *fmt = value < 0 ? "-%1h%2m" : "%1h%2m";
            s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
            s = s.arg(fabs(hr), 2, 'f', 0, '0').arg(fabs(mi), 3 + digits, 'f', digits, '0');
        }
        else if ((*this) == CTimeUnit::minsec())
        {
            digits -= 2;
            Q_ASSERT(digits >= 0);
            double mi = CMathUtils::trunc(value);
            double se = CMathUtils::trunc((value - mi) * 100.0);
            const char *fmt = value < 0 ? "-%2m%3s" : "%2m%3s";
            s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
            s = s.arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 3 + digits, 'f', digits, '0');
        }
        else
        {
            s = this->CMeasurementUnit::makeRoundedQStringWithUnit(value, digits, withGroupSeparator, i18n);
        }
        return s;
    }

} // namespace
