// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_UPPERCASEVALIDATOR_H
#define SWIFT_GUI_UPPERCASEVALIDATOR_H

#include <QStringList>
#include <QValidator>

#include "gui/swiftguiexport.h"

namespace swift::gui
{
    //! Forces uppercase
    class SWIFT_GUI_EXPORT CUpperCaseValidator : public QValidator
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
} // namespace swift::gui
#endif // SWIFT_GUI_UPPERCASEVALIDATOR_H
