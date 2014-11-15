/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "uppercasevalidator.h"

namespace BlackGui
{
    CUpperCaseValidator::CUpperCaseValidator(QObject *parent) : QValidator(parent)
    { }

    CUpperCaseValidator::CUpperCaseValidator(int minLength, int maxLength, QObject *parent) : QValidator(parent),
        m_minLength(minLength), m_maxLength(maxLength)
    { }

    CUpperCaseValidator::CUpperCaseValidator(bool optionalValue, int minLength, int maxLength, QObject *parent) : QValidator(parent),
        m_minLength(minLength), m_maxLength(maxLength), m_optionalValue(optionalValue)
    { }

    QValidator::State CUpperCaseValidator::validate(QString &input, int &pos) const
    {
        Q_UNUSED(input);
        Q_UNUSED(pos);
        fixup(input);

        if (m_optionalValue && input.isEmpty()) { return Acceptable; }
        if (input.length() > m_maxLength) { return Invalid; }
        if (input.length() < m_minLength) { return Intermediate; }
        return Acceptable;
    }

    void CUpperCaseValidator::fixup(QString &input) const
    {
        if (input.isEmpty()) { return; }
        input = input.toUpper();
    }

} // namespace
