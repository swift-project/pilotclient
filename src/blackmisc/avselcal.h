/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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
            //! Default constructor.
            CSelcal() {}

            //! Constructor.
            CSelcal(const QString &code) : m_code(code.trimmed()) {}

            /*!
             * Constructor.
             * Needed to disambiguate implicit conversion from string literal.
             */
            CSelcal(const char *code) : m_code(code) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Is valid?
            bool isValid() const { return CSelcal::isValidCode(this->m_code); }

            //! Get SELCAL code
            const QString &getCode() const { return this->m_code; }

            /*!
             * List of 4 frequencies, if list is empty SELCAL code is not valid
             * \return  either 4 frequencies, or empty list
             */
            QList<BlackMisc::PhysicalQuantities::CFrequency> getFrequencies() const;

            //! Equal operator ==
            bool operator ==(const CSelcal &other) const;

            //! Unequal operator !=
            bool operator !=(const CSelcal &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

            //! Equals given string
            bool equalsString(const QString &code) const;

            //! Valid SELCAL characters
            static const QString &validCharacters();

            //! Is given character a valid SELCAL characer?
            static bool isValidCharacter(QChar c);

            //! Valid SELCAL code?
            static bool isValidCode(const QString &code);

            //! Audio frequency for character
            static const BlackMisc::PhysicalQuantities::CFrequency &audioFrequencyEquivalent(QChar c);

            //! All valid code pairs: AB, AC, AD ...
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
