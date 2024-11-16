// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_PQSTRING_H
#define SWIFT_MISC_PQ_PQSTRING_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::physical_quantities, CPqString)

namespace swift::misc::physical_quantities
{
    /*!
     * Represents a physical quantity by a string
     * \details Used to parse strings into physical quantities, validate strings
     * \sa swift::misc::PhysicalQuantity
     */
    class SWIFT_MISC_EXPORT CPqString : public CValueObject<CPqString>
    {
    public:
        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Number separators / group separators
        enum SeparatorMode
        {
            SeparatorQtDefault, //!< 100000.00 no group separator
            SeparatorLocale, //!< depending on QLocale, e.g. 100.000,00 in Germany
            SeparatorBestGuess //!< try to figure out
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
        static CVariant parseToVariant(const QString &value, SeparatorMode mode = SeparatorQtDefault);

        //! Parse into concrete type
        template <class PQ>
        static PQ parse(const QString &value, SeparatorMode mode = SeparatorQtDefault)
        {
            if (value.isEmpty()) { return PQ::null(); }
            if (value.contains("null", Qt::CaseInsensitive)) { return PQ::null(); }
            const CVariant qv = parseToVariant(value, mode);
            if (!qv.isNull() && qv.canConvert<PQ>()) { return qv.value<PQ>(); }
            return PQ::null();
        }

        //! Locale aware parsing
        static double parseNumber(const QString &number, bool &success, SeparatorMode mode = SeparatorBestGuess);

    private:
        QString m_string;

        SWIFT_METACLASS(
            CPqString,
            SWIFT_METAMEMBER(string));
    };
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CPqString)

#endif // guard
