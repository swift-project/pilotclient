/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/stringutils.h"

#include <Qt>
#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAirportIcaoCode)

namespace BlackMisc::Aviation
{
    QString CAirportIcaoCode::convertToQString(bool /** i18n **/) const
    {
        return m_icaoCode;
    }

    bool CAirportIcaoCode::hasValidIcaoCode(bool strict) const
    {
        return CAirportIcaoCode::isValidIcaoDesignator(this->getIcaoCode(), strict);
    }

    bool CAirportIcaoCode::equalsString(const QString &icaoCode) const
    {
        CAirportIcaoCode other(icaoCode);
        return other == (*this);
    }

    QString CAirportIcaoCode::unifyAirportCode(const QString &icaoCode)
    {
        const QString code = icaoCode.trimmed().toUpper();
        if (!validCodeLength(icaoCode.length(), false)) return {};
        if (containsChar(code, [](QChar c) { return !c.isLetterOrNumber(); })) { return {}; }
        return code;
    }

    bool CAirportIcaoCode::isValidIcaoDesignator(const QString &icaoCode, bool strict)
    {
        const QString icao = unifyAirportCode(icaoCode);
        return validCodeLength(icao.length(), strict);
    }

    bool CAirportIcaoCode::containsNumbers(const QString &icaoCode)
    {
        return (containsChar(icaoCode, [](QChar c) { return c.isDigit(); }));
    }

    QVariant CAirportIcaoCode::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        return CValueObject::propertyByIndex(index);
    }

    void CAirportIcaoCode::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAirportIcaoCode>();
            return;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    int CAirportIcaoCode::comparePropertyByIndex(CPropertyIndexRef index, const CAirportIcaoCode &compareValue) const
    {
        Q_UNUSED(index);
        return m_icaoCode.compare(compareValue.getIcaoCode(), Qt::CaseInsensitive);
    }

    bool CAirportIcaoCode::validCodeLength(int l, bool strict)
    {
        // FAA code 3
        // ICAO code 4
        if (strict) { return l == 4; }
        return l >= 3 && l <= 6;

        // https://en.wikipedia.org/wiki/Location_identifier#FAA_identifier says can be up to 5 characters
        // https://en.wikipedia.org/wiki/ICAO_airport_code#Pseudo_ICAO-codes says France has some 6-character airport codes
        // and ZZZZ can be used in a flight plan as ICAO code for any airport that doesn't have one
    }
} // namespace
