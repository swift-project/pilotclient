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
#include "blackmisc/datastore.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc
{
    namespace Aviation
    {
        class CAirlineIcaoCode;

        //! Value object for ICAO classification
        class BLACKMISC_EXPORT CAircraftIcaoCode :
            public CValueObject<CAircraftIcaoCode>,
            public BlackMisc::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAircraftDesignator = BlackMisc::CPropertyIndex::GlobalIndexCAircraftIcaoCode,
                IndexIataCode,
                IndexFamily,
                IndexCombinedAircraftType,
                IndexManufacturer,
                IndexModelDescription,
                IndexWtc,
                IndexIsRealworld,
                IndexIsMilitary,
                IndexIsLegacy,
                IndexIsVtol,
                IndexRank,
                IndexDesignatorManufacturer    //!< designator and manufacturer
            };

            //! Default constructor.
            CAircraftIcaoCode() = default;

            //! Constructor.
            CAircraftIcaoCode(const QString &designator, const QString &combinedType = "");

            //! Constructor
            CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer,
                              const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank);

            //! Constructor
            CAircraftIcaoCode(const QString &icao, const QString &iata, const QString &combinedType, const QString &manufacturer,
                              const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank);

            //! Get ICAO designator, e.g. "B737"
            const QString &getDesignator() const { return m_designator; }

            //! Set ICAO designator, e.g. "B737"
            void setDesignator(const QString &icaoDesignator) { this->m_designator = icaoDesignator.trimmed().toUpper(); }

            //! Aircraft designator?
            bool hasDesignator() const;

            //! Valid aircraft designator?
            bool hasValidDesignator() const;

            //! Has designator and designator is not "ZZZZ"
            bool hasKnownDesignator() const;

            //! Special designator
            bool hasSpecialDesignator() const;

            //! IATA code
            const QString &getIataCode() const { return m_iataCode; }

            //! Set IATA code
            void setIataCode(const QString &iata) { this->m_iataCode = iata.toUpper().trimmed(); }

            //! Has IATA code?
            bool hasIataCode() const { return !this->m_iataCode.isEmpty(); }

            //! IATA code same as designator?
            bool isIataSameAsDesignator() const;

            //! Family (e.g. A350)
            const QString &getFamily() const { return m_family; }

            //! Set family
            void setFamily(const QString &family) { this->m_family = family.toUpper().trimmed(); }

            //! Has family?
            bool hasFamily() const { return !this->m_family.isEmpty(); }

            //! Family same as designator?
            bool isFamilySameAsDesignator() const;

            //! Get type, e.g. "L2J"
            const QString &getCombinedType() const { return this->m_combinedType; }

            //! Combined type available?
            bool hasValidCombinedType() const;

            //! Get engine type, e.g. "J"
            QString getEngineType() const;

            //! Engine count if any, -1 if no value is set
            int getEngineCount() const;

            //! Engine count as string, if not available ""
            QString getEngineCountString() const;

            //! Aircraft type, such a L(andplane), S(eaplane), H(elicopter)
            QString getAircraftType() const;

            //! Set type
            void setCombinedType(const QString &type) { this->m_combinedType = type.trimmed().toUpper(); }

            //! Get model description, e.g. "A-330-200"
            const QString &getModelDescription() const { return m_modelDescription; }

            //! Matches given combined code
            //! \remark * can be used as wildcard, e.g. L*J, L**
            bool matchesCombinedCode(const QString &combinedCode) const;

            //! Designator + Manufacturer
            QString getDesignatorManufacturer() const;

            //! Set the model description
            void setModelDescription(const QString &modelDescription) { m_modelDescription = modelDescription.trimmed(); }

            //! Has model description
            bool hasModelDescription() const { return !this->m_modelDescription.isEmpty(); }

            //! Get manufacturer, e.g. "Airbus"
            const QString &getManufacturer() const { return m_manufacturer; }

            //! Set the manufacturer
            void setManufacturer(const QString &manufacturer) { m_manufacturer = manufacturer.trimmed(); }

            //! Manufacturer
            bool hasManufacturer() const;

            //! Get WTC
            const QString &getWtc() const { return m_wtc; }

            //! Set WTC
            void setWtc(const QString &wtc) { m_wtc = wtc.trimmed().toUpper(); }

            //! Valid WTC code?
            bool hasValidWtc() const { return !m_wtc.isEmpty(); }

            //! Is VTOL aircraft (helicopter, tilt wing)
            bool isVtol() const;

            //! Military?
            bool isMilitary() const { return m_military; }

            //! Real world aircraft?
            bool isRealWorld() const { return m_realWorld; }

            //! Legacy aircraft (no current ICAO code)
            bool isLegacyAircraft() const { return m_legacy; }

            //! Flags
            void setCodeFlags(bool military, bool legacy, bool realWorld);

            //! Military
            void setMilitary(bool military);

            //! Real world
            void setRealWorld(bool realWorld);

            //! Legacy
            void setLegacy(bool legacy);

            //! Ranking
            int getRank() const { return m_rank; }

            //! Ranking
            QString getRankString() const;

            //! Ranking
            void setRank(int rank);

            //! Combined ICAO descriptive string with key
            QString getCombinedIcaoStringWithKey() const;

            //! Combined IATA descriptive string with key
            QString getCombinedIataStringWithKey() const;

            //! Combined family descriptive string with key
            QString getCombinedFamilyStringWithKey() const;

            //! All data set?
            bool hasCompleteData() const;

            //! Matches designator string?
            bool matchesDesignator(const QString &designator) const;

            //! Matches IATA string?
            bool matchesIataCode(const QString &iata) const;

            //! Matches family?
            bool matchesFamily(const QString &family) const;

            //! Matches ICAO or IATA code
            bool matchesDesignatorOrIata(const QString &icaoOrIata) const;

            //! Matches ICAO, IATA, family?
            bool matchesDesignatorIataOrFamily(const QString &icaoIataOrFamily) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Compare for index
            int comparePropertyByIndex(const CAircraftIcaoCode &compareValue, const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Update missing parts
            void updateMissingParts(const CAircraftIcaoCode &otherIcaoCode);

            //! Validate data
            BlackMisc::CStatusMessageList validate() const;

            //! Valid designator?
            static bool isValidDesignator(const QString &designator);

            //! Valid combined type
            static bool isValidCombinedType(const QString &combinedType);

            //! Valid WTC code?
            static bool isValidWtc(const QString &candidate);

            //! The unassigned designator ("ZZZZ")
            static const QString &getUnassignedDesignator();

            //! List of the special designators ("ZZZZ", "UHEL", ...)
            static const QStringList &getSpecialDesignators();

            //! Normalize designator, remove illegal characters
            static const QString normalizeDesignator(const QString candidate);

            //! From our database JSON format
            static CAircraftIcaoCode fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        private:
            QString m_designator;         //!< "B737"
            QString m_iataCode;           //!< "320"
            QString m_family;             //!< "A350" (not a real ICAO code, but a family)
            QString m_combinedType;       //!< "L2J"
            QString m_manufacturer;       //!< "Airbus"
            QString m_modelDescription;   //!< "A-330-200"
            QString m_wtc;                //!< wake turbulence "M","H" "L/M", "L", we only use the one letter versions
            bool m_realWorld = true;      //!< real world aircraft
            bool m_legacy = false;        //!< legacy code
            bool m_military = false;      //!< military aircraft?
            int m_rank = 10;              //!< rank among same codes

            //! Create a combined string like L2J
            static QString createdCombinedString(const QString &type, const QString &engineCount, const QString &engine);

            //! Create a combined string like L2J
            static QString createdCombinedString(const QString &type, int engineCount, const QString &engine);

            BLACK_METACLASS(
                CAircraftIcaoCode,
                BLACK_METAMEMBER(dbKey),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(designator),
                BLACK_METAMEMBER(iataCode),
                BLACK_METAMEMBER(family),
                BLACK_METAMEMBER(combinedType),
                BLACK_METAMEMBER(manufacturer),
                BLACK_METAMEMBER(modelDescription),
                BLACK_METAMEMBER(wtc),
                BLACK_METAMEMBER(military),
                BLACK_METAMEMBER(realWorld),
                BLACK_METAMEMBER(legacy),
                BLACK_METAMEMBER(rank)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoCode)

#endif // guard
