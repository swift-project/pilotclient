/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTICAO_H
#define BLACKMISC_AVIATION_AIRCRAFTICAO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for ICAO classification
        class BLACKMISC_EXPORT CAircraftIcao : public CValueObject<CAircraftIcao>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAircraftDesignator = BlackMisc::CPropertyIndex::GlobalIndexCAircraftIcao,
                IndexCombinedAircraftType,
                IndexAirlineDesignator,
                IndexAircraftColor,
                IndexAsString,
                IndexIsVtol
            };

            //! Default constructor.
            CAircraftIcao() = default;

            //! Constructor.
            explicit CAircraftIcao(const QString &icao) : m_aircraftDesignator(icao.trimmed().toUpper()) {}

            //! Constructor.
            //! \param icao "B737"
            //! \param airline "DLH"
            CAircraftIcao(const QString &icao, const QString &airline);

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

            //! Get ICAO designator, e.g. "B737"
            const QString &getAircraftDesignator() const { return m_aircraftDesignator; }

            //! Set ICAO designator, e.g. "B737"
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftDesignator = icaoDesignator.trimmed().toUpper(); }

            //! Aircraft designator?
            bool hasAircraftDesignator() const;

            //! Has designator and designator is not "ZZZZ"
            bool hasKnownAircraftDesignator() const;

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
            QString getEngineType() const;

            //! As string for GUI representation by index
            //! \remarks Different from toQString()
            QString asString() const;

            //! Set type
            void setAircraftCombinedType(const QString &type) { this->m_aircraftCombinedType = type.trimmed().toUpper(); }

            //! Missing parts from another ICAO object
            void updateMissingParts(const CAircraftIcao &icao);

            //! Matches wildcard icao object
            bool matchesWildcardIcao(const CAircraftIcao &otherIcao) const;

            //! Is VTOL aircraft
            bool isVtol() const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Valid designator?
            static bool isValidDesignator(const QString &designator);

            //! Valid combined type
            static bool isValidCombinedType(const QString &combinedType);

            //! Valid designator?
            static bool isValidAirlineDesignator(const QString &airline);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftIcao)
            QString m_aircraftDesignator;   //!< "B737"
            QString m_aircraftCombinedType; //!< "L2J"
            QString m_airlineDesignator;    //!< "DLH"
            QString m_livery;
            QString m_aircraftColor;        //!< RGB Hex "330044"
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcao)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftIcao, (
                                   o.m_aircraftDesignator,
                                   o.m_aircraftCombinedType,
                                   o.m_airlineDesignator,
                                   o.m_livery,
                                   o.m_aircraftColor
                               ))


#endif // guard
