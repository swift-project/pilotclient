/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_UPPERCASEVALIDATOR_H
#define BLACKGUI_UPPERCASEVALIDATOR_H

#include "blackgui/blackguiexport.h"
#include <QStringList>
#include <QValidator>

namespace BlackGui
{
    //! Forces uppercase
    class BLACKGUI_EXPORT CUpperCaseValidator : public QValidator
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CUpperCaseValidator(QObject *parent = nullptr);

        //! Constructor
        CUpperCaseValidator(int minLength, int maxLength, QObject *parent = nullptr);

        //! Constructor
        CUpperCaseValidator(bool optionalValue, int minLength, int maxLength, QObject *parent = nullptr);

        //! Set restrictions
        void setRestrictions(const QStringList &restrictions) { m_restrictions = restrictions; }

        //! Allowed characters
        void setAllowedCharacters(const QString &chars) { m_allowedCharacters = chars.toUpper(); }

        //! Set the allowed characters as 0-9 and A-Z
        void setAllowedCharacters09AZ();

        //! \copydoc QValidator::validate
        virtual State validate(QString &input, int &pos) const override;

        //! \copydoc QValidator::fixup
        virtual void fixup(QString &input) const override;

    private:
        bool m_optionalValue = false; //!< allow empty
        int m_minLength = 0;
        int m_maxLength = 32678; //!< standard length
        QString m_allowedCharacters; //!< allowedCharacters
        QStringList m_restrictions; //!< list of allowed strings
    };
}
#endif // guard
