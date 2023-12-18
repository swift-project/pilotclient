// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTICAOCODE_H
#define BLACKMISC_AVIATION_AIRCRAFTICAOCODE_H

#include "blackmisc/aviation/aircraftcategory.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QStringList>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftIcaoCode)

namespace BlackMisc::Aviation
{
    //! Value object for ICAO classification
    class BLACKMISC_EXPORT CAircraftIcaoCode :
        public CValueObject<CAircraftIcaoCode>,
        public Db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAircraftDesignator = CPropertyIndexRef::GlobalIndexCAircraftIcaoCode,
            IndexIataCode,
            IndexFamily,
            IndexCombinedAircraftType,
            IndexManufacturer,
            IndexModelDescription,
            IndexModelIataDescription,
            IndexModelSwiftDescription,
            IndexCombinedDescription,
            IndexWtc,
            IndexIsRealworld,
            IndexIsMilitary,
            IndexIsLegacy,
            IndexIsVtol,
            IndexRank,
            IndexCategory,
            IndexDesignatorManufacturer //!< designator and manufacturer
        };

        //! Default constructor.
        CAircraftIcaoCode() {}

        //! Constructor.
        CAircraftIcaoCode(const QString &icao, const QString &combinedType = "");

        //! Constructor
        CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer,
                          const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank);

        //! Constructor
        CAircraftIcaoCode(const QString &icao, const QString &iata, const QString &combinedType, const QString &manufacturer,
                          const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank);

        //! Constructor
        CAircraftIcaoCode(const QString &icao, const QString &iata, const QString &family, const QString &combinedType, const QString &manufacturer,
                          const QString &model, const QString &modelIata, const QString &modelSwift, const QString &wtc, bool realworld, bool legacy, bool military, int rank);

        //! Get ICAO designator, e.g. "B737"
        const QString &getDesignator() const { return m_designator; }

        //! Designator and DB key
        QString getDesignatorDbKey() const;

        //! Designator + Manufacturer
        QString getDesignatorManufacturer() const;

        //! Set ICAO designator, e.g. "B737"
        void setDesignator(const QString &icaoDesignator) { m_designator = icaoDesignator.trimmed().toUpper(); }

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
        void setIataCode(const QString &iata) { m_iataCode = iata.toUpper().trimmed(); }

        //! Has IATA code?
        bool hasIataCode() const { return !m_iataCode.isEmpty(); }

        //! IATA code same as designator?
        bool isIataSameAsDesignator() const;

        //! Family (e.g. A350)
        const QString &getFamily() const { return m_family; }

        //! Set family
        void setFamily(const QString &family) { m_family = family.toUpper().trimmed(); }

        //! Has family?
        bool hasFamily() const { return !m_family.isEmpty(); }

        //! Family same as designator?
        bool isFamilySameAsDesignator() const;

        //! Get type, e.g. "L2J"
        const QString &getCombinedType() const { return m_combinedType; }

        //! Combined type available?
        bool hasValidCombinedType() const;

        //! Get engine type, e.g. "J"
        QString getEngineType() const;

        //! Get engine type, e.g. "J"
        QChar getEngineTypeChar() const;

        //! Engine count if any, -1 if no value is set
        int getEnginesCount() const;

        //! Engine count as string, if not available ""
        QString getEngineCountString() const;

        //! Aircraft type, such a L(andplane), S(eaplane), H(elicopter)
        QString getAircraftType() const;

        //! Aircraft type, such a L(andplane), S(eaplane), H(elicopter)
        QChar getAircraftTypeChar() const;

        //! Set type
        void setCombinedType(const QString &type) { m_combinedType = type.trimmed().toUpper(); }

        //! Get IACO model description, e.g. "A-330-200"
        const QString &getModelDescription() const { return m_modelDescription; }

        //! Get IATA model description
        const QString &getModelIataDescription() const { return m_modelIataDescription; }

        //! Get swift model description
        const QString &getModelSwiftDescription() const { return m_modelSwiftDescription; }

        //! Combined description
        QString getCombinedModelDescription() const;

        //! Matches any of the (unempty) descriptions
        bool matchesAnyDescription(const QString &candidate) const;

        //! Matches given combined code
        //! \remark * can be used as wildcard, e.g. L*J, L**
        bool matchesCombinedType(const QString &combinedType) const;

        //! Matches combined type and
        bool matchesCombinedTypeAndManufacturer(const QString &combinedType, const QString &manufacturer) const;

        //! Set the model description (ICAO description)
        void setModelDescription(const QString &modelDescription) { m_modelDescription = modelDescription.trimmed(); }

        //! Set the alternative IATA model description
        void setModelIataDescription(const QString &modelDescription) { m_modelIataDescription = modelDescription.trimmed(); }

        //! Set the  alternative swift model description
        void setModelSwiftDescription(const QString &modelDescription) { m_modelSwiftDescription = modelDescription.trimmed(); }

        //! Has model description?
        bool hasModelDescription() const { return !m_modelDescription.isEmpty(); }

        //! Has IATA model description?
        bool hasModelIataDescription() const { return !m_modelIataDescription.isEmpty(); }

        //! Has swift model description?
        bool hasModelSwiftDescription() const { return !m_modelSwiftDescription.isEmpty(); }

        //! Get manufacturer, e.g. "Airbus"
        const QString &getManufacturer() const { return m_manufacturer; }

        //! Set the manufacturer
        void setManufacturer(const QString &manufacturer) { m_manufacturer = manufacturer.trimmed(); }

        //! Manufacturer
        bool hasManufacturer() const;

        //! Matching the manufacturer?
        bool matchesManufacturer(const QString &manufacturer) const;

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

        //! Is DB duplicate? This means a redundant ICAO DB entry.
        //! \see https://aviation.stackexchange.com/q/37848/4024
        bool isDbDuplicate() const;

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

        //! Combined ICAO descriptive string with category and key
        QString getCombinedIcaoCategoryStringWithKey() const;

        //! Combined IATA descriptive string with key
        QString getCombinedIataStringWithKey() const;

        //! Combined family descriptive string with key
        QString getCombinedFamilyStringWithKey() const;

        //! Get category
        const CAircraftCategory &getCategory() const { return m_category; }

        //! Has category?
        bool hasCategory() const;

        //! Set category
        void setCategory(const CAircraftCategory &category) { m_category = category; }

        //! Set category id
        void setCategoryId(int id) { m_category.setDbKey(id); }

        //! All data set?
        bool hasCompleteData() const;

        //! Matches designator string?
        bool matchesDesignator(const QString &designator, int fuzzyMatch = -1, int *result = nullptr) const;

        //! Matches IATA string?
        //! \remark IATA codes are only 3 characters, so using fuzzy search might yield bad results
        bool matchesIataCode(const QString &iata, int fuzzyMatch = -1, int *result = nullptr) const;

        //! Matches family?
        bool matchesFamily(const QString &family, int fuzzyMatch = -1, int *result = nullptr) const;

        //! Matches ICAO or IATA code
        bool matchesDesignatorOrIata(const QString &icaoOrIata) const;

        //! Matches ICAO, IATA, family?
        bool matchesDesignatorIataOrFamily(const QString &icaoIataOrFamily) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftIcaoCode &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Update missing parts
        void updateMissingParts(const CAircraftIcaoCode &otherIcaoCode);

        //! Validate data
        CStatusMessageList validate() const;

        //! As a brief HTML summary (e.g. used in tooltips)
        QString asHtmlSummary() const;

        //! Considers rank, manufacturer and family 0..100
        //! \remark normally used with a selected set of ICAO codes or combined types
        int calculateScore(const CAircraftIcaoCode &otherCode, CStatusMessageList *log = nullptr) const;

        //! Guess aircraft model parameters
        //! \remark values will not be overridden, pass null values to obtain guessed values
        void guessModelParameters(PhysicalQuantities::CLength &guessedCGOut, PhysicalQuantities::CSpeed &guessedVRotateOut) const;

        //! Null ICAO?
        bool isNull() const;

        //! NULL object
        static const CAircraftIcaoCode &null();

        //! Valid designator?
        static bool isValidDesignator(const QString &designator);

        //! Valid combined type
        static bool isValidCombinedType(const QString &combinedType);

        //! The unassigned designator ("ZZZZ")
        static const QString &getUnassignedDesignator();

        //! Get the glider designator
        static const QString &getGliderDesignator();

        //! Unassigned ICAO code "ZZZZ"
        static const CAircraftIcaoCode &unassignedIcao();

        //! List of the special designators ("ZZZZ", "UHEL", ...)
        static const QStringList &getSpecialDesignators();

        //! Normalize designator, remove illegal characters
        static QString normalizeDesignator(const QString &candidate);

        //! Create relaxed combined codes, e.g "L2J" -> "L3J", ...
        static QStringList alternativeCombinedCodes(const QString &combinedCode);

        //! Engine tye is Electric, Piston, TurboProp
        static bool isEPTEngineType(const QChar engineType);

        //! Specialized log message for matching / reverse lookup
        //! \threadsafe
        static CStatusMessage logMessage(
            const CAircraftIcaoCode &icaoCode,
            const QString &message, const QStringList &extraCategories = {},
            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        //! Specialized log for matching / reverse lookup
        //! \threadsafe
        static void addLogDetailsToList(CStatusMessageList *log, const CAircraftIcaoCode &icao,
                                        const QString &message, const QStringList &extraCategories = {},
                                        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        //! From our database JSON format
        static CAircraftIcaoCode fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        static constexpr int DesignatorMinLength = 2; //!< designator length (min)
        static constexpr int DesignatorMaxLength = 5; //!< designator length (max)

    private:
        QString m_designator; //!< "B737"
        QString m_iataCode; //!< "320"
        QString m_family; //!< "A350" (not a real ICAO code, but a family)
        QString m_combinedType; //!< "L2J"
        QString m_manufacturer; //!< "Airbus"
        QString m_modelDescription; //!< "A-330-200", the ICAO description
        QString m_modelIataDescription; //!< alternative IATA description
        QString m_modelSwiftDescription; //!< alternative swift description
        QString m_wtc; //!< wake turbulence like "M","H" "L/M", "L", "J", we only use the one letter versions
        bool m_realWorld = true; //!< real world aircraft
        bool m_legacy = false; //!< legacy code
        bool m_military = false; //!< military aircraft?
        int m_rank = 10; //!< rank among same codes (0 is best)
        CAircraftCategory m_category; //!< aircraft category

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
            BLACK_METAMEMBER(modelIataDescription),
            BLACK_METAMEMBER(modelSwiftDescription),
            BLACK_METAMEMBER(category),
            BLACK_METAMEMBER(wtc),
            BLACK_METAMEMBER(military),
            BLACK_METAMEMBER(realWorld),
            BLACK_METAMEMBER(legacy),
            BLACK_METAMEMBER(rank)
        );
    };

    //! Id/CAircraftIcaoCode map
    using AircraftIcaoIdMap = QMap<int, CAircraftIcaoCode>;

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoCode)

#endif // guard
