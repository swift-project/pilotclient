/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/pqunits.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        using BlackMisc::Math::CMath;

        /*
         * Rounded to QString
         */
        QString CAngleUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
        {
            if (digits < 0) digits = this->getDisplayDigits();
            QString s;
            if ((*this) == CAngleUnit::sexagesimalDeg())
            {
                digits -= 4;
                Q_ASSERT(digits >= 0);
                double de = CMath::trunc(value);
                double mi = CMath::trunc((value - de) * 100.0);
                double se = CMath::trunc((value - de - mi / 100.0) * 1000000) / 100.0;
                const char *fmt = value < 0 ? "-%L1 %L2 %L3" : "%L1 %L2 %L3";
                s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
                s = s.arg(fabs(de), 0, 'f', 0).arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 2, 'f', digits, '0');
            }
            else if ((*this) == CAngleUnit::sexagesimalDegMin())
            {
                digits -= 2;
                Q_ASSERT(digits >= 0);
                double de = CMath::trunc(value);
                double mi = CMath::trunc((value - de) * 100.0);
                const char *fmt = value < 0 ? "-%L1 %L2" : "%L1 %L2";
                s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
                s = s.arg(fabs(de), 0, 'f', 0).arg(fabs(mi), 2, 'f', digits, '0');
            }
            else
            {
                s = this->CMeasurementUnit::makeRoundedQStringWithUnit(value, digits, i18n);
            }
            return s;
        }

        /*
         * Rounded to QString
         */
        QString CTimeUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
        {
            if (digits < 0) digits = this->getDisplayDigits();
            QString s;
            if ((*this) == CTimeUnit::hms())
            {
                digits -= 4;
                Q_ASSERT(digits >= 0);
                double hr = CMath::trunc(value);
                double mi = CMath::trunc((value - hr) * 100.0);
                double se = CMath::trunc((value - hr - mi / 100.0) * 1000000) / 100.0;
                const char *fmt = value < 0 ? "-%L1h%L2m%L3s" : "%L1h%L2m%L3s";
                s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
                s = s.arg(fabs(hr), 2, 'f', 0, '0').arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 2, 'f', digits, '0');
            }
            else if ((*this) == CTimeUnit::hrmin())
            {
                digits -= 2;
                Q_ASSERT(digits >= 0);
                double hr = CMath::trunc(value);
                double mi = CMath::trunc((value - hr) * 100.0);
                const char *fmt = value < 0 ? "-%L1h%L2m" : "%L1h%L2m";
                s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
                s = s.arg(fabs(hr), 2, 'f', 0, '0').arg(fabs(mi), 2, 'f', digits, '0');
            }
            else if ((*this) == CTimeUnit::minsec())
            {
                digits -= 2;
                Q_ASSERT(digits >= 0);
                double mi = CMath::trunc(value);
                double se = CMath::trunc((value - mi) * 100.0);
                const char *fmt = value < 0 ? "-%L2m%L3s" : "%L2m%L3s";
                s = i18n ? QCoreApplication::translate("CMeasurementUnit", fmt) : fmt;
                s = s.arg(fabs(mi), 2, 'f', 0, '0').arg(fabs(se), 2, 'f', digits, '0');
            }
            else
            {
                s = this->CMeasurementUnit::makeRoundedQStringWithUnit(value, digits, i18n);
            }
            return s;
        }

    } // namespace
} // namespace
