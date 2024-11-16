// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_SELCAL_H
#define SWIFT_MISC_AVIATION_SELCAL_H

#include <QChar>
#include <QList>
#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSelcal)
namespace swift::misc::physical_quantities
{
    class CFrequency;
}
namespace swift::misc::aviation
{
    /*!
     * Value object for SELCAL.
     * \see http://en.wikipedia.org/wiki/SELCAL
     * \see http://www.asri.aero/our-services/selcal/ User Guide
     */
    class SWIFT_MISC_EXPORT CSelcal : public CValueObject<CSelcal>
    {
    public:
        //! Default constructor.
        CSelcal() = default;

        //! Constructor.
        CSelcal(const QString &code) : m_code(unifyCode(code)) {}

        //! Constructor needed to disambiguate implicit conversion from string literal.
        CSelcal(const char *code) : m_code(unifyCode(code)) {}

        //! Is valid?
        bool isValid() const { return isValidCode(m_code); }

        //! Get SELCAL code
        const QString &getCode() const { return m_code; }

        //! List of 4 frequencies, if list is empty SELCAL code is not valid
        //! \return  either 4 frequencies, or empty list
        QList<physical_quantities::CFrequency> getFrequencies() const;

        //! Equals given string
        bool equalsString(const QString &code) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Valid SELCAL characters
        static const QString &validCharacters();

        //! Is given character a valid SELCAL characer?
        static bool isValidCharacter(QChar c);

        //! Valid SELCAL code?
        static bool isValidCode(const QString &code);

        //! All valid code pairs: AB, AC, AD ...
        static const QStringList &codePairs();

        //! Unify SELCAL code by removing illegal characters
        static QString unifyCode(const QString &selcalCandidate);

    private:
        //! Audio frequency for character
        static const physical_quantities::CFrequency &audioFrequencyEquivalent(QChar c);

        //! All audio frequencies
        static const QList<physical_quantities::CFrequency> &audioFrequencyEquivalents();

        QString m_code;

        SWIFT_METACLASS(
            CSelcal,
            SWIFT_METAMEMBER(code));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CSelcal)

#endif // guard
