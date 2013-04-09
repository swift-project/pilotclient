/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/aviotransponder.h"

namespace BlackMisc
{
namespace Aviation
{

/**
 * Valid values?
 */
bool CTransponder::validValues() const
{
    if (this->isDefaultValue()) return true; // special case
    if (this->m_transponderCode < 0 || this->m_transponderCode > 7777) return false;

    // check each digit
    qint32 tc = this->m_transponderCode;
    qint32 d;
    while (tc > 7) {
        d = (tc % 10);
        if (d > 7) return false;
        tc /= 10;
    }
    return true;
}

/**
 * Validate
 */
bool CTransponder::validate(bool strict) const
{
    if (this->isDefaultValue()) return true;
    bool valid = this->validValues();
    if (!strict) return valid;
    Q_ASSERT_X(valid, "CTransponder::validate", "illegal values");
    if (!valid) throw std::range_error("Illegal values in CTransponder::validate");
    return true;
}

/**
 * String representation
 */
QString CTransponder::stringForStreamingOperator() const
{
    QString s = this->getName();
    s = s.append(" ").append(this->getTransponderCodeFormatted()).append(" ").append(this->getModeAsString());
    return s;
}

/**
 * Mode as readable string
 */
QString CTransponder::getModeAsString() const
{
    QString m;
    switch (this->m_transponderMode) {
    case StateIdent:
        m = "Ident";
        break;
    case StateStandby:
        m = "Standby";
        break;
    case ModeC:
        m = "Mode C";
        break;
    case ModeMil1:
        m = "Mil.Mode 1";
        break;
    case ModeMil2:
        m = "Mil.Mode 2";
        break;
    case ModeMil3:
        m = "Mil.Mode 3";
        break;
    case ModeMil4:
        m = "Mil.Mode 4";
        break;
    case ModeMil5:
        m = "Mil.Mode 5";
        break;
    default:
        throw std::range_error("Illegal Transponder Mode");
    }
    return m;
}

/**
 * Formatted transponder code
 */
QString CTransponder::getTransponderCodeFormatted() const
{
    QString f("0000");
    f = f.append(QString::number(this->m_transponderCode));
    return f.right(4);
}

} // namespace
} // namespace
