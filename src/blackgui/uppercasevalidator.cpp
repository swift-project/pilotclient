// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/stringutils.h"
#include "blackgui/uppercasevalidator.h"
#include <QString>
#include <QtGlobal>

using namespace swift::misc;

namespace BlackGui
{
    CUpperCaseValidator::CUpperCaseValidator(QObject *parent) : QValidator(parent)
    {}

    CUpperCaseValidator::CUpperCaseValidator(int minLength, int maxLength, QObject *parent) : QValidator(parent),
                                                                                              m_minLength(minLength), m_maxLength(maxLength)
    {
        if (minLength < 1) { m_optionalValue = true; };
    }

    CUpperCaseValidator::CUpperCaseValidator(bool optionalValue, int minLength, int maxLength, QObject *parent) : QValidator(parent),
                                                                                                                  m_optionalValue(optionalValue), m_minLength(minLength), m_maxLength(maxLength)
    {
        if (minLength < 1) { m_optionalValue = true; };
    }

    void CUpperCaseValidator::setAllowedCharacters09AZ()
    {
        static const QString chars("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        this->setAllowedCharacters(chars);
    }

    QValidator::State CUpperCaseValidator::validate(QString &input, int &pos) const
    {
        Q_UNUSED(input);
        Q_UNUSED(pos);
        this->fixup(input);

        if (m_optionalValue && input.isEmpty()) { return Acceptable; }
        if (input.length() > m_maxLength) { return Invalid; }
        if (input.length() < m_minLength) { return Intermediate; }
        if (!m_restrictions.isEmpty())
        {
            bool valid = false;
            for (const QString &r : m_restrictions)
            {
                if (r.startsWith(input))
                {
                    valid = true;
                    break;
                }
            }
            if (!valid) { return Invalid; }
        }
        return Acceptable;
    }

    void CUpperCaseValidator::fixup(QString &input) const
    {
        if (input.isEmpty()) { return; }
        input = input.toUpper();
        if (!m_allowedCharacters.isEmpty())
        {
            input = removeIfNotInString(input, m_allowedCharacters);
        }
    }
} // namespace
