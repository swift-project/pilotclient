/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/pqbase.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

// -----------------------------------------------------------------------
// --- Measurement unit --------------------------------------------------
// -----------------------------------------------------------------------

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
            if (this->isNull() || unit.isNull()) return -1; // models the previous behaviour of using -1 as a sentinel value
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

        /*!
         * \brief Register metadata of unit and quantity
         */
        void CMeasurementUnit::registerMetadata()
        {
            qRegisterMetaType<CMeasurementUnit>();
            qDBusRegisterMetaType<CMeasurementUnit>();
        }



    } // namespace
} // namespace
