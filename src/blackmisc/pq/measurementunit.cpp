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
            return this->m_name == other.m_name;
        }

        bool CMeasurementUnit::operator !=(const CMeasurementUnit &other) const
        {
            return !(other == *this);
        }

        double CMeasurementUnit::convertFrom(double value, const CMeasurementUnit &unit) const
        {
            if (this->isNull() || unit.isNull()) return 0;
            if (this->m_toDefault == unit.m_toDefault && this->m_fromDefault == unit.m_fromDefault) return value;
            return this->m_fromDefault(unit.m_toDefault(value));
        }

        QString CMeasurementUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
        {
            return this->makeRoundedQString(value, digits).append(this->getSymbol(i18n));
        }

        double CMeasurementUnit::roundValue(double value, int digits) const
        {
            if (digits < 0) digits = this->m_displayDigits;
            return CMathUtils::round(value, digits);
        }

        QString CMeasurementUnit::makeRoundedQString(double value, int digits, bool /* i18n */) const
        {
            if (digits < 0) digits = this->m_displayDigits;
            double v = CMathUtils::round(value, digits);
            QString s = QLocale::system().toString(v, 'f', digits);
            return s;
        }

    } // namespace
} // namespace
