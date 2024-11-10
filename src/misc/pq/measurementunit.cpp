// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/pq/measurementunit.h"
#include "misc/math/mathutils.h"
#include <QLocale>

using namespace swift::misc::math;

namespace swift::misc::physical_quantities
{
    double CMeasurementUnit::convertFrom(double value, const CMeasurementUnit &unit) const
    {
        if (this->isNull() || unit.isNull()) return 0;
        if (m_data->m_toDefault == unit.m_data->m_toDefault && m_data->m_fromDefault == unit.m_data->m_fromDefault) return value;
        return m_data->m_fromDefault(unit.m_data->m_toDefault(value));
    }

    QString CMeasurementUnit::makeRoundedQStringWithUnit(double value, int digits, bool withGroupSeparator, bool i18n) const
    {
        return this->makeRoundedQString(value, digits, withGroupSeparator).append(this->getSymbol(i18n));
    }

    double CMeasurementUnit::roundValue(double value, int digits) const
    {
        if (digits < 0) { digits = m_data->m_displayDigits; }
        return CMathUtils::round(value, digits);
    }

    double CMeasurementUnit::roundToEpsilon(double value) const
    {
        if (qFuzzyIsNull(getEpsilon()) || this->isNull()) { return value; }
        return CMathUtils::roundEpsilon(value, this->getEpsilon());
    }

    QString CMeasurementUnit::makeRoundedQString(double value, int digits, bool withGroupSeparator, bool i18n) const
    {
        Q_UNUSED(i18n);
        if (digits < 0) { digits = m_data->m_displayDigits; }
        const double v = CMathUtils::round(value, digits);

        // create locale without separator
        static const QLocale localeWithoutSeparator = [] {
            QLocale q = QLocale::system();
            q.setNumberOptions(QLocale::OmitGroupSeparator);
            return q;
        }();

        const QString s = (withGroupSeparator ? QLocale::system() : localeWithoutSeparator).toString(v, 'f', digits);
        return s;
    }
} // namespace
