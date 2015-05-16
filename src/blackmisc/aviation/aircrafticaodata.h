/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTICAODATA_H
#define BLACKMISC_AVIATION_AIRCRAFTICAODATA_H

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for ICAO classification (airline ICAO, aircraft ICAO, livery ..)
        class BLACKMISC_EXPORT CAircraftIcaoData : public CValueObject<CAircraftIcaoData>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAircraftIcao = BlackMisc::CPropertyIndex::GlobalIndexCAircraftIcaoData,
                IndexAirlineIcao,
                IndexAircraftColor,
                IndexAsString,
            };

            //! Default constructor.
            CAircraftIcaoData() = default;

            //! Constructor.
            //! \param icao "B737"
            //! \param airline "DLH"
            CAircraftIcaoData(const QString &icao, const QString &airline = "");

            //! Constructor.
            CAircraftIcaoData(const BlackMisc::Aviation::CAircraftIcaoCode &icaoAircraft, const BlackMisc::Aviation::CAirlineIcaoCode &icaoAirline, const QString &color = "");

            //! Get ICAO designator, e.g. "B737"
            const QString &getAircraftDesignator() const { return m_aircraftIcao.getDesignator(); }

            //! Get aircraft ICAO object
            const BlackMisc::Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const { return this->m_aircraftIcao; }

            //! Set ICAO designator, e.g. "B737"
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftIcao.setDesignator(icaoDesignator); }

            //! Aircraft designator?
            bool hasAircraftDesignator() const { return this->m_aircraftIcao.hasDesignator(); }

            //! Has designator and designator is not "ZZZZ"
            bool hasKnownAircraftDesignator() const { return (this->m_aircraftIcao.hasKnownDesignator()); }

            //! Get airline, e.g. "DLH"
            const QString &getAirlineDesignator() const { return this->m_airlineIcao.getDesignator(); }

            //! Get airline ICAO object
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const { return this->m_airlineIcao; }

            //! Set airline, e.g. "DLH"
            void setAirlineDesignator(const QString &icaoDesignator) { this->m_airlineIcao.setDesignator(icaoDesignator); }

            //! Airline available?
            bool hasAirlineDesignator() const { return this->m_airlineIcao.hasDesignator(); }

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
            const QString &getAircraftCombinedType() const { return this->m_aircraftIcao.getCombinedType(); }

            //! Combined type available?
            bool hasAircraftCombinedType() const { return this->m_aircraftIcao.hasCombinedType(); }

            //! Get engine type, e.g. "J"
            QString getEngineType() const { return this->m_aircraftIcao.getEngineType(); }

            //! As string for GUI representation by index
            //! \remarks Different from toQString()
            QString asString() const;

            //! Set type
            void setAircraftCombinedType(const QString &type) { this->m_aircraftIcao.setCombinedType(type); }

            //! Missing parts from another ICAO object
            void updateMissingParts(const CAircraftIcaoData &icao);

            //! Matches wildcard icao object
            bool matchesWildcardIcao(const CAircraftIcaoData &otherIcao) const;

            //! Is VTOL aircraft
            bool isVtol() const { return m_aircraftIcao.isVtol(); }

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftIcaoData)
            BlackMisc::Aviation::CAircraftIcaoCode m_aircraftIcao;   //!< "B737", ...
            BlackMisc::Aviation::CAirlineIcaoCode  m_airlineIcao;    //!< "DLH", ...
            QString m_livery;
            QString m_aircraftColor;                                 //!< RGB Hex "330044"
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoData)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftIcaoData, (
                                   o.m_aircraftIcao,
                                   o.m_airlineIcao,
                                   o.m_livery,
                                   o.m_aircraftColor
                               ))

#endif // guard
