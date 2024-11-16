// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRPORTICAOCODE_H
#define SWIFT_MISC_AVIATION_AIRPORTICAOCODE_H

#include <tuple>

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAirportIcaoCode)

namespace swift::misc::aviation
{
    //! Value object encapsulating information of airport ICAO data.
    class SWIFT_MISC_EXPORT CAirportIcaoCode : public CValueObject<CAirportIcaoCode>
    {
    public:
        //! Default constructor.
        CAirportIcaoCode() {}

        //! Constructor
        CAirportIcaoCode(const QString &icaoCode) : m_icaoCode(CAirportIcaoCode::unifyAirportCode(icaoCode)) {}

        //! Constructor, needed to disambiguate implicit conversion from string literal.
        CAirportIcaoCode(const char *icaoCode) : m_icaoCode(CAirportIcaoCode::unifyAirportCode(icaoCode)) {}

        //! Is empty?
        bool isEmpty() const { return this->m_icaoCode.isEmpty(); }

        //! Has valid code?
        bool hasValidIcaoCode(bool strict) const;

        //! Get code.
        const QString &asString() const { return this->m_icaoCode; }

        //! Get ICAO code
        QString getIcaoCode() const { return m_icaoCode; }

        //! Equals callsign string?
        bool equalsString(const QString &icaoCode) const;

        //! Unify code
        static QString unifyAirportCode(const QString &icaoCode);

        //! Valid ICAO designator
        static bool isValidIcaoDesignator(const QString &icaoCode, bool strict);

        //! Containing numbers (normally indicator for small airfield/strip)
        static bool containsNumbers(const QString &icaoCode);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAirportIcaoCode &compareValue) const;

        //! Valid code lenght
        static bool validCodeLength(int l, bool strict);

    private:
        QString m_icaoCode;

        SWIFT_METACLASS(
            CAirportIcaoCode,
            SWIFT_METAMEMBER(icaoCode, 0, CaseInsensitiveComparison));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAirportIcaoCode)

#endif // guard
