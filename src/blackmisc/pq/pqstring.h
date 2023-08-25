// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PQ_PQSTRING_H
#define BLACKMISC_PQ_PQSTRING_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::PhysicalQuantities, CPqString)

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Represents a physical quantity by a string
     * \details Used to parse strings into physical quantities, validate strings
     * \sa BlackMisc::PhysicalQuantity
     */
    class BLACKMISC_EXPORT CPqString : public CValueObject<CPqString>
    {
    public:
        //! \copydoc BlackMisc::Mixin::String::toQString
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
            if (!qv.isNull() && qv.canConvert<PQ>())
            {
                return qv.value<PQ>();
            }
            return PQ::null();
        }

        //! Locale aware parsing
        static double parseNumber(const QString &number, bool &success, SeparatorMode mode = SeparatorBestGuess);

    private:
        QString m_string;

        BLACK_METACLASS(
            CPqString,
            BLACK_METAMEMBER(string)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPqString)

#endif // guard
