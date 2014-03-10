/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_SELCAL_H
#define BLACKMISC_SELCAL_H
#include "blackmisc/pqfrequency.h"
#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object for SELCAL.
         * \see http://en.wikipedia.org/wiki/SELCAL
         * \see http://www.asri.aero/our-services/selcal/ User Guide
         */
        class CSelcal : public BlackMisc::CValueObject
        {

        public:
            /*!
             * Default constructor.
             */
            CSelcal() {}

            /*!
             * Constructor.
             */
            CSelcal(const QString &code) : m_code(code.trimmed()) {}

            /*!
             * Constructor.
             * Needed to disambiguate implicit conversion from string literal.
             */
            CSelcal(const char *code) : m_code(code) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Is valid?
             * \return
             */
            bool isValid() const { return CSelcal::isValidCode(this->m_code); }

            /*!
             * Get SELCAL code
             */
            const QString &getCode() const { return this->m_code; }

            /*!
             * \brief List of 4 frequencies, if list is empty SELCAL code is not valid
             * \return  either 4 frequencies, or empty list
             */
            QList<BlackMisc::PhysicalQuantities::CFrequency> getFrequencies() const;

            /*!
             * \brief Equal operator ==
             */
            bool operator ==(const CSelcal &other) const;

            /*!
             * \brief Unequal operator !=
             */
            bool operator !=(const CSelcal &other) const;

            /*!
             * \copydoc CValueObject::getValueHash
             */
            virtual uint getValueHash() const override;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Equals given string
             */
            bool equalsString(const QString &code) const;

            /*!
             * \brief Valid SELCAL characters
             */
            static const QString &validCharacters();

            /*!
             * \brief Is given character a valid SELCAL characer?
             */
            static bool isValidCharacter(QChar c);

            /*!
             * Valid SELCAL code?
             */
            static bool isValidCode(const QString &code);

            /*!
             * \brief Audio frequency for character
             */
            static const BlackMisc::PhysicalQuantities::CFrequency &audioFrequencyEquivalent(QChar c);

            /*!
             * \brief All valid code pairs: AB, AC, AD ...
             */
            static const QStringList &codePairs();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSelcal)
            QString m_code;
            static QList<BlackMisc::PhysicalQuantities::CFrequency> frequencyEquivalents;
            static QStringList allCodePairs;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CSelcal, (o.m_code))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CSelcal)

#endif // guard
