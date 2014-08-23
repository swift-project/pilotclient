/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "blackmisc/pqbase.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*
         * Equal operator
         */
        bool CMeasurementUnit::operator ==(const CMeasurementUnit &other) const
        {
            if (this == &other) return true;
            return this->m_name == other.m_name;
        }

        /*
         * Unequal operator
         */
        bool CMeasurementUnit::operator !=(const CMeasurementUnit &other) const
        {
            return !(other == *this);
        }

        /*
         * Conversion
         */
        double CMeasurementUnit::convertFrom(double value, const CMeasurementUnit &unit) const
        {
            if (this->isNull() || unit.isNull()) return 0;
            if (this->m_converter == unit.m_converter) return value;
            return this->m_converter->fromDefault(unit.m_converter->toDefault(value));
        }

        /*
         * Value to QString with unit, e.g. "5.00m"
         * @return
         */
        QString CMeasurementUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
        {
            return this->makeRoundedQString(value, digits).append(this->getSymbol(i18n));
        }

        /*
         * Value rounded
         */
        double CMeasurementUnit::roundValue(double value, int digits) const
        {
            if (digits < 0) digits = this->m_displayDigits;
            return CMath::round(value, digits);
        }

        /*
         * Rounded to QString
         */
        QString CMeasurementUnit::makeRoundedQString(double value, int digits, bool /* i18n */) const
        {
            if (digits < 0) digits = this->m_displayDigits;
            double v = CMath::round(value, digits);
            QString s = QLocale::system().toString(v, 'f', digits);
            return s;
        }

        /*
         * metaTypeId
         */
        int CMeasurementUnit::getMetaTypeId() const
        {
            return qMetaTypeId<CMeasurementUnit>();
        }

        /*
         * is a
         */
        bool CMeasurementUnit::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CMeasurementUnit>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CMeasurementUnit::compareImpl(const CValueObject &/*otherBase*/) const
        {
            qFatal("not implemented");
            return 0;
        }

        /*
         * Register metadata of unit and quantity
         */
        void CMeasurementUnit::registerMetadata()
        {
            qRegisterMetaType<CMeasurementUnit>();
            qDBusRegisterMetaType<CMeasurementUnit>();
        }

    } // namespace
} // namespace
