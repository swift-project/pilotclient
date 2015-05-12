/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTICAOCODE_H
#define BLACKMISC_AVIATION_AIRCRAFTICAOCODE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for ICAO classification
        class BLACKMISC_EXPORT CAircraftIcaoCode : public CValueObject<CAircraftIcaoCode>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAircraftDesignator = BlackMisc::CPropertyIndex::GlobalIndexCAircraftIcaoCode,
                IndexCombinedAircraftType,
                IndexManufacturer,
                IndexModelDescription,
                IndexWtc,
                IndexIsRealworld,
                IndexIsMilitary,
                IndexIsLegacy,
                IndexIsVtol
            };

            //! Default constructor.
            CAircraftIcaoCode() = default;

            //! Constructor
            CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer,
                              const QString &model, const QString &wtc, bool military, bool realworld, bool legacy);

            //! Get ICAO designator, e.g. "B737"
            const QString &getAircraftDesignator() const { return m_aircraftDesignator; }

            //! Set ICAO designator, e.g. "B737"
            void setAircraftDesignator(const QString &icaoDesignator) { this->m_aircraftDesignator = icaoDesignator.trimmed().toUpper(); }

            //! Aircraft designator?
            bool hasAircraftDesignator() const;

            //! Has designator and designator is not "ZZZZ"
            bool hasKnownAircraftDesignator() const;

            //! Get type, e.g. "L2J"
            const QString &getAircraftCombinedType() const { return this->m_aircraftCombinedType; }

            //! Combined type available?
            bool hasAircraftCombinedType() const { return this->getAircraftCombinedType().length() == 3; }

            //! Get engine type, e.g. "J"
            QString getEngineType() const;

            //! Set type
            void setAircraftCombinedType(const QString &type) { this->m_aircraftCombinedType = type.trimmed().toUpper(); }

            //! Get model description, e.g. "A-330-200"
            const QString &getModelDescription() const { return m_modelDescription; }

            //! Set the model description
            void setModelDescription(const QString &modelDescription) { m_modelDescription = modelDescription.trimmed(); }

            //! Get manufacturer, e.g. "Airbus"
            const QString &getManufacturer() const { return m_manufacturer; }

            //! Set the manufacturer
            void setManufacturer(const QString &manufacturer) { m_manufacturer = manufacturer.trimmed(); }

            //! Get WTC
            const QString &getWtc() const { return m_wtc; }

            //! Set WTC
            void setWtc(const QString &wtc) { m_wtc = wtc.trimmed().toUpper(); }

            //! Valid WTC code?
            bool hasValidWtc() const { return !m_wtc.isEmpty(); }

            //! Is VTOL aircraft (helicopter, tilt wing)
            bool isVtol() const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Valid designator?
            static bool isValidDesignator(const QString &designator);

            //! Valid combined type
            static bool isValidCombinedType(const QString &combinedType);

            //! From our database JSON format
            static CAircraftIcaoCode fromDatabaseJson(const QJsonObject &json);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftIcaoCode)
            QString m_aircraftDesignator;   //!< "B737"
            QString m_aircraftCombinedType; //!< "L2J"
            QString m_manufacturer;         //!< "Airbus"
            QString m_modelDescription;     //!< "A-330-200"
            QString m_wtc;                  //!< wake turbulence "M","H" "L/M", "L"
            bool m_military = false;
            bool m_realworld = true;        //!< real world aircraft
            bool m_legacy = true;           //!< legacy code

        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoCode)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftIcaoCode, (
                                   o.m_aircraftDesignator,
                                   o.m_aircraftCombinedType,
                                   o.m_manufacturer,
                                   o.m_modelDescription,
                                   o.m_wtc,
                                   o.m_military,
                                   o.m_realworld,
                                   o.m_legacy
                               ))

#endif // guard
