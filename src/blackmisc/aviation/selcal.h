/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_SELCAL_H
#define BLACKMISC_AVIATION_SELCAL_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QChar>
#include <QList>
#include <QMetaType>
#include <QString>

class QStringList;

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CSelcal)
namespace BlackMisc::PhysicalQuantities
{
    class CFrequency;
}
namespace BlackMisc::Aviation
{
    /*!
     * Value object for SELCAL.
     * \see http://en.wikipedia.org/wiki/SELCAL
     * \see http://www.asri.aero/our-services/selcal/ User Guide
     */
    class BLACKMISC_EXPORT CSelcal : public CValueObject<CSelcal>
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
        QList<PhysicalQuantities::CFrequency> getFrequencies() const;

        //! Equals given string
        bool equalsString(const QString &code) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
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
        static const PhysicalQuantities::CFrequency &audioFrequencyEquivalent(QChar c);

        //! All audio frequencies
        static const QList<PhysicalQuantities::CFrequency> &audioFrequencyEquivalents();

        QString m_code;

        BLACK_METACLASS(
            CSelcal,
            BLACK_METAMEMBER(code)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CSelcal)

#endif // guard
