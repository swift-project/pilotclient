/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFTICAO_H
#define BLACKMISC_AIRCRAFTICAO_H

#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object for ICAO classification
         */
        class CAircraftIcao : public BlackMisc::CValueObject
        {
        public:
            /*!
             * Default constructor.
             */
            CAircraftIcao() {}

            /*!
             * Constructor.
             * \param icao
             * \param type
             * \param airline
             * \param livery
             * \param color
             */
            CAircraftIcao(const QString &icao, const QString &type, const QString &airline, const QString &livery, const QString &color)
                : m_designator(icao.trimmed().toUpper()), m_type(type.trimmed().toUpper()), m_airline(airline.trimmed().toUpper()),
                  m_livery(livery.trimmed().toUpper()), m_color(color.trimmed().toUpper()) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Get ICAO designator, e.g. "B737"
            const QString &getDesignator() const { return m_designator; }

            //! Set callsign
            void setDesignator(const QString &icaoDesignator) { this->m_designator = icaoDesignator.trimmed().toUpper(); }

            //! \brief Get airline
            const QString &getAirline() const { return this->m_airline; }

            //! \brief Set airline
            void setAirline(const QString &airline) { this->m_airline = airline.trimmed().toUpper(); }

            //! \brief Airline available?
            bool hasAirline() const { return !this->m_airline.isEmpty(); }

            //! \brief Get livery
            const QString &getLivery() const { return this->m_livery; }

            //! \brief Set livery
            void setLivery(const QString &livery) { this->m_livery = livery.trimmed().toUpper(); }

            //! \brief has livery?
            bool hasLivery() const { return !this->m_livery.isEmpty(); }

            //! \brief Get livery or color
            const QString &getLiveryOrColor() const { return this->hasLivery() ? this->m_livery : this->m_color; }

            //! \brief Get color
            const QString &getColor() const { return this->m_color; }

            //! \brief Set color
            void setColor(const QString &color) { this->m_color = color.trimmed().toUpper(); }

            //! \brief Color available?
            bool hasColor() const { return !this->m_color.isEmpty(); }

            //! \brief Get type
            const QString &getType() const { return this->m_type; }

            //! \brief Get engine type
            QString getEngineType() const
            {
                if (this->m_type.length() != 3) return "";
                return this->m_type.right(1);
            }

            /*!
             * \brief As string for GUI representation by index
             * \remarks Different from toQString()
             */
            QString asString() const;

            //! \brief Set type
            void setType(const QString &type) { this->m_type = type.trimmed().toUpper(); }

            //! \brief Equal operator ==
            bool operator ==(const CAircraftIcao &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAircraftIcao &other) const;

            //! \brief Value hash
            virtual uint getValueHash() const override;

            //! \brief Register metadata
            static void registerMetadata();

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexIcaoDesignator = 2000, // used, so it can be chained in aircraft
                IndexType,
                IndexAirline,
                IndexColor,
                IndexAsString
            };

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::propertyByIndex(index, i18n)
            virtual QString propertyByIndexAsString(int index, bool i18n) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const;

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
            QString m_designator;
            QString m_type;
            QString m_airline;
            QString m_livery;
            QString m_color;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcao)

#endif // guard
