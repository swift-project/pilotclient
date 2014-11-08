/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQSTRING_H
#define BLACKMISC_PQSTRING_H

#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QVariant>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * \brief Represents a physical quantity by a string
         * \details Used to parse strings into physical quantities, validate strings
         * \sa BlackMisc::PhysicalQuantity
         */
        class CPqString : public CValueObjectStdTuple<CPqString>
        {
        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        public:
            //! Number separators / group separators
            enum SeparatorMode
            {
                SeparatorsCLocale, //!< 100,000.00
                SeparatorsLocale, //!< depending on QLocale, e.g. 100.000,00 in Germany
                SeparatorsBestGuess //!< try to figure out
            };

            //! Group and digit separator
            enum SeparatorIndex
            {
                Group,
                Digit
            };

            //! Default constructor
            CPqString() = default;

            //! Constructor, for values such as 10km/h
            CPqString(const QString &value) : m_string(value) {}

            //! Parse a string value like "100m", "10.3Mhz"
            static QVariant parseToVariant(const QString &value, SeparatorMode mode = SeparatorsCLocale);

            //! Parse into concrete type
            template <class PQ> static PQ parse(const QString &value, SeparatorMode mode = SeparatorsCLocale)
            {
                PQ invalid;
                invalid.setNull();
                if (value.isEmpty()) return invalid;
                QVariant qv = CPqString::parseToVariant(value, mode);
                if (!qv.isNull() && qv.canConvert<PQ>())
                {
                    return qv.value<PQ>();
                }
                return invalid;
            }

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CPqString)
            QString m_string;
        };

    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::PhysicalQuantities::CPqString, (o.m_string))
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPqString)

#endif // guard
