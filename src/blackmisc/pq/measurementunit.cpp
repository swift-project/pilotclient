/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/math/mathutils.h"
#include <QLocale>

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        bool CMeasurementUnit::operator ==(const CMeasurementUnit &other) const
        {
            if (this == &other) return true;
            return m_data->m_name == other.m_data->m_name;
        }

        bool CMeasurementUnit::operator !=(const CMeasurementUnit &other) const
        {
            return !(other == *this);
        }

        double CMeasurementUnit::convertFrom(double value, const CMeasurementUnit &unit) const
        {
            if (this->isNull() || unit.isNull()) return 0;
            if (m_data->m_toDefault == unit.m_data->m_toDefault && m_data->m_fromDefault == unit.m_data->m_fromDefault) return value;
            return m_data->m_fromDefault(unit.m_data->m_toDefault(value));
        }

        QString CMeasurementUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
        {
            return this->makeRoundedQString(value, digits).append(this->getSymbol(i18n));
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

        QString CMeasurementUnit::makeRoundedQString(double value, int digits, bool i18n) const
        {
            Q_UNUSED(i18n);
            if (digits < 0) digits = m_data->m_displayDigits;
            const double v = CMathUtils::round(value, digits);
            const QString s = QLocale::system().toString(v, 'f', digits);
            return s;
        }
    } // namespace
} // namespace
