/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AIRPORTICAO_H
#define BLACKMISC_AIRPORTICAO_H
#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! \brief Value object encapsulating information of a callsign.
        class CAirportIcao : public BlackMisc::CValueObject
        {

        public:
            //! Default constructor.
            CAirportIcao() {}

            //! Constructor
            CAirportIcao(const QString &icaoCode) : m_icaoCode(CAirportIcao::unifyAirportCode(icaoCode))
            {}

            //! Constructor, needed to disambiguate implicit conversion from string literal.
            CAirportIcao(const char *icaoCode)
                : m_icaoCode(CAirportIcao::unifyAirportCode(icaoCode))
            {}

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Is empty?
            bool isEmpty() const { return this->m_icaoCode.isEmpty(); }

            //! Get code.
            const QString &asString() const { return this->m_icaoCode; }

            //! Get ICAO code
            QString getIcaoCode() const { return m_icaoCode; }

            //! Equals callsign string?
            bool equalsString(const QString &icaoCode) const;

            //! Equal operator ==
            bool operator ==(const CAirportIcao &other) const;

            //! Unequal operator !=
            bool operator !=(const CAirportIcao &other) const;

            //! Less than operator < for sorting
            bool operator <(const CAirportIcao &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

            //! Unify code
            static QString unifyAirportCode(const QString &icaoCode);

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirportIcao)
            QString m_icaoCode;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirportIcao, (o.m_icaoCode))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportIcao)

#endif // guard
