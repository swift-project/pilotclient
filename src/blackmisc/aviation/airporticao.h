/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRPORTICAO_H
#define BLACKMISC_AVIATION_AIRPORTICAO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of airport ICAO data.
        class BLACKMISC_EXPORT CAirportIcao : public CValueObject<CAirportIcao>
        {
        public:
            //! Default constructor.
            CAirportIcao() = default;

            //! Constructor
            CAirportIcao(const QString &icaoCode) : m_icaoCode(CAirportIcao::unifyAirportCode(icaoCode)) {}

            //! Constructor, needed to disambiguate implicit conversion from string literal.
            CAirportIcao(const char *icaoCode) : m_icaoCode(CAirportIcao::unifyAirportCode(icaoCode)) {}

            //! Is empty?
            bool isEmpty() const { return this->m_icaoCode.isEmpty(); }

            //! Get code.
            const QString &asString() const { return this->m_icaoCode; }

            //! Get ICAO code
            QString getIcaoCode() const { return m_icaoCode; }

            //! Equals callsign string?
            bool equalsString(const QString &icaoCode) const;

            //! Unify code
            static QString unifyAirportCode(const QString &icaoCode);

            //! Valid ICAO designator
            static bool isValidIcaoDesignator(const QString &icaoCode);

            //! \copydoc CValueObject::convertToQString()
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirportIcao)
            QString m_icaoCode;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirportIcao, (
    attr(o.m_icaoCode, flags<CaseInsensitiveComparison>())
))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportIcao)

#endif // guard
