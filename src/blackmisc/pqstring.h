/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQSTRING_H
#define BLACKMISC_PQSTRING_H

#include "blackmisc/valueobject.h"
#include <QVariant>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * \brief Represents a physical quantity by a string
         * \details Used to parse strings into physical quantities, validate strings
         * \sa BlackMisc::PhysicalQuantity
         */
        class CPqString : public BlackMisc::CValueObject
        {
        public:

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CPqString)
            QString m_string;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

        public:
            //! Default constructor
            CPqString() {}

            /*!
             * Constructor
             * \param value such as 10km/h
             */
            CPqString(const QString &value) : m_string(value) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \copydoc CValueObject::getMetaTypeId
            int getMetaTypeId() const override;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! Equal operator ==
            bool operator ==(const CPqString &other) const;

            //! Unequal operator !=
            bool operator !=(const CPqString &other) const;

            //! Register metadata
            static void registerMetadata();

            //! Parse a string value like "100m", "10.3Mhz"
            static QVariant parseToVariant(const QString &value);

            //! Parse into concrete type
            template <class PQ> static PQ parse(const QString &value)
            {
                PQ invalid;
                invalid.setNull();
                if (value.isEmpty()) return invalid;
                QVariant qv = CPqString::parseToVariant(value);
                if (!qv.isNull() && qv.canConvert<PQ>())
                {
                    return qv.value<PQ>();
                }
                return invalid;
            }
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::PhysicalQuantities::CPqString, (o.m_string))
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPqString)

#endif // guard
