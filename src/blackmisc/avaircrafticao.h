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
         * Value object encapsulating information for ICAO classification
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

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Get ICAO designator, e.g. "B737"
             * \return
             */
            const QString &getDesignator() const { return m_designator; }

            /*!
             * Set callsign
             * \param callsign
             */
            void setDesignator(const QString &icaoDesignator) { this->m_designator = icaoDesignator.trimmed().toUpper(); }

            /*!
             * \brief Get airline
             * \return
             */
            const QString &getAirline() const { return this->m_airline; }

            /*!
             * \brief Set airline
             * \param airline
             */
            void setAirline(const QString &airline) { this->m_airline = airline.trimmed().toUpper(); }

            /*!
             * \brief Airline?
             * \return
             */
            bool hasAirline() const { return !this->m_airline.isEmpty(); }

            /*!
             * \brief Get livery
             * \return
             */
            const QString &getLivery() const { return this->m_livery; }

            /*!
             * \brief Set airline
             * \param livery
             */
            void setLivery(const QString &livery) { this->m_livery = livery.trimmed().toUpper(); }

            /*!
             * \brief Livery?
             * \return
             */
            bool hasLivery() const { return !this->m_livery.isEmpty(); }

            /*!
             * \brief Get livery or color
             * \return
             */
            const QString &getLiveryOrColor() const { return this->hasLivery() ? this->m_livery : this->m_color; }

            /*!
             * \brief Get color
             * \return
             */
            const QString &getColor() const { return this->m_color; }

            /*!
             * \brief Set color
             * \param color
             */
            void setColor(const QString &color) { this->m_color = color.trimmed().toUpper(); }

            /*!
             * \brief Color?
             * \return
             */
            bool hasColor() const { return !this->m_color.isEmpty(); }

            /*!
             * \brief Get type
             * \return
             */
            const QString &getType() const { return this->m_type; }

            /*!
             * \brief Get engine type
             * \return
             */
            QString getEngineType() const
            {
                if (this->m_type.length() != 3) return "";
                return this->m_type.right(1);
            }

            /*!
             * \brief As string for GUI representation
             * \return
             */
            QString asString() const;

            /*!
             * \brief Set type
             * \param type
             */
            void setType(const QString &type) { this->m_type = type.trimmed().toUpper(); }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CAircraftIcao &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CAircraftIcao &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
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

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            virtual QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index as string
             * \param index
             * \param i18n
             * \return
             */
            virtual QString propertyByIndexAsString(int index, bool i18n) const;

            /*!
             * \brief Property by index (setter)
             * \param variant
             * \param index
             */
            virtual void setPropertyByIndex(const QVariant &variant, int index);

        protected:
            /*!
             * \brief Rounded value as string
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

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
