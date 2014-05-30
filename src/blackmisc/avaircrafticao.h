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
            //! Default constructor.
            CAircraftIcao() {}

            //! Default constructor.
            explicit CAircraftIcao(const QString &icao) : m_aircraftDesignator(icao.trimmed().toUpper()) {}

            /*!
             * Constructor.
             * \param icao "B737"
             * \param combinedType "L2J"
             * \param airline "DLH"
             * \param livery "FREIGHT"
             * \param color "CCFFDD" (RGB)
             */
            CAircraftIcao(const QString &icao, const QString &combinedType, const QString &airline, const QString &livery, const QString &color)
                : m_aircraftDesignator(icao.trimmed().toUpper()), m_aircraftCombinedType(combinedType.trimmed().toUpper()), m_airlineDesignator(airline.trimmed().toUpper()),
                  m_livery(livery.trimmed().toUpper()), m_aircraftColor(color.trimmed().toUpper()) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Get ICAO designator, e.g. "B737"
            const QString &getAircraftDesignator() const { return m_aircraftDesignator; }

            //! Set ICAO designator, e.g. "B737"
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftDesignator = icaoDesignator.trimmed().toUpper(); }

            //! Aircraft designator?
            bool hasAircraftDesignator() const { return !this->m_aircraftDesignator.isEmpty(); }

            //! Get airline, e.g. "DLH"
            const QString &getAirlineDesignator() const { return this->m_airlineDesignator; }

            //! Set airline, e.g. "DLH"
            void setAirlineDesignator(const QString &icaoDesignator) { this->m_airlineDesignator = icaoDesignator.trimmed().toUpper(); }

            //! Airline available?
            bool hasAirlineDesignator() const { return !this->m_airlineDesignator.isEmpty(); }

            //! Airline and Aircraft designator?
            bool hasAircraftAndAirlineDesignator() const { return this->hasAirlineDesignator() && this->hasAircraftDesignator(); }

            //! Get livery
            const QString &getLivery() const { return this->m_livery; }

            //! Set livery
            void setLivery(const QString &livery) { this->m_livery = livery.trimmed().toUpper(); }

            //! has livery?
            bool hasLivery() const { return !this->m_livery.isEmpty(); }

            //! Get livery or color
            const QString &getLiveryOrColor() const { return this->hasLivery() ? this->m_livery : this->m_aircraftColor; }

            //! Get color (RGB hex)
            const QString &getAircraftColor() const { return this->m_aircraftColor; }

            //! Set color (RGB hex)
            void setAircraftColor(const QString &color) { this->m_aircraftColor = color.trimmed().toUpper(); }

            //! Color available?
            bool hasAircraftColor() const { return !this->m_aircraftColor.isEmpty(); }

            //! Get type, e.g. "L2J"
            const QString &getAircraftCombinedType() const { return this->m_aircraftCombinedType; }

            //! Combined type available?
            bool hasAircraftCombinedType() const { return this->getAircraftCombinedType().length() == 3; }

            //! Get engine type, e.g. "J"
            QString getEngineType() const
            {
                if (this->m_aircraftCombinedType.length() != 3) return "";
                return this->m_aircraftCombinedType.right(1);
            }

            /*!
             * \brief As string for GUI representation by index
             * \remarks Different from toQString()
             */
            QString asString() const;

            //! Set type
            void setAircraftCombinedType(const QString &type) { this->m_aircraftCombinedType = type.trimmed().toUpper(); }

            //! Equal operator ==
            bool operator ==(const CAircraftIcao &other) const;

            //! Unequal operator !=
            bool operator !=(const CAircraftIcao &other) const;

            //! Value hash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexAircraftDesignator = 2000, // used, so it can be chained in aircraft
                IndexCombinedAircraftType,
                IndexAirlineDesignator,
                IndexAircraftColor,
                IndexAsString
            };

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::propertyByIndex(index, i18n)
            virtual QString propertyByIndexAsString(int index, bool i18n) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

            //! Valid designator?
            static bool isValidDesignator(const QString &designator);

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
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftIcao)
            QString m_aircraftDesignator; //!< "B737"
            QString m_aircraftCombinedType; //!< "L2J"
            QString m_airlineDesignator; //!< "DLH"
            QString m_livery;
            QString m_aircraftColor; //!< RGB Hex "330044"
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftIcao, (o.m_aircraftDesignator, o.m_aircraftCombinedType, o.m_airlineDesignator, o.m_livery, o.m_aircraftColor))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcao)

#endif // guard
