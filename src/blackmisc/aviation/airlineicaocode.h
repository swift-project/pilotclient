// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRLINEICAOCODE_H
#define BLACKMISC_AVIATION_AIRLINEICAOCODE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/country.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QMap>
#include <QSet>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAirlineIcaoCode)

namespace BlackMisc::Aviation
{
    class CCallsign;

    //! Value object for ICAO classification
    class BLACKMISC_EXPORT CAirlineIcaoCode :
        public CValueObject<CAirlineIcaoCode>,
        public Db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAirlineDesignator = CPropertyIndexRef::GlobalIndexCAirlineIcaoCode,
            IndexIataCode,
            IndexAirlineName,
            IndexAirlineCountryIso,
            IndexAirlineCountry,
            IndexAirlineIconHTML,
            IndexTelephonyDesignator,
            IndexGroupId,
            IndexGroupName,
            IndexGroupDesignator,
            IndexIsVirtualAirline,
            IndexIsOperating,
            IndexIsMilitary,
            IndexDesignatorNameCountry,
        };

        //! Default constructor.
        CAirlineIcaoCode() = default;

        //! Constructor.
        CAirlineIcaoCode(const QString &airlineDesignator);

        //! Constructor.
        CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const CCountry &country, const QString &telephony, bool virtualAirline, bool operating);

        //! Get airline, e.g. "DLH"
        const QString &getDesignator() const { return m_designator; }

        //! Get airline, e.g. "DLH", but "VMVA" for virtual airlines
        QString getVDesignator() const;

        //! Get VDesignator plus key
        QString getVDesignatorDbKey() const;

        //! Set airline, e.g. "DLH"
        void setDesignator(const QString &icaoDesignator);

        //! IATA code
        const QString &getIataCode() const { return m_iataCode; }

        //! Set IATA code
        void setIataCode(const QString &iataCode) { m_iataCode = iataCode.trimmed().toUpper(); }

        //! Get country, e.g. "FR"
        const QString &getCountryIso() const { return m_country.getIsoCode(); }

        //! Get country, e.g. "FRANCE"
        const CCountry &getCountry() const { return m_country; }

        //! Designator and DB key
        QString getDesignatorDbKey() const;

        //! Combined string designator, name, country
        QString getDesignatorNameCountry() const;

        //! Set country
        void setCountry(const CCountry &country) { m_country = country; }

        //! Get name, e.g. "Lufthansa"
        const QString &getName() const { return m_name; }

        //! \copydoc simplifyNameForSearch
        QString getSimplifiedName() const;

        //! Name plus key, e.g. "Lufthansa (3421)"
        QString getNameWithKey() const;

        //! Set name
        void setName(const QString &name) { m_name = name.trimmed(); }

        //! Telephony designator such as "Speedbird"
        const QString &getTelephonyDesignator() const { return m_telephonyDesignator; }

        //! Telephony designator such as "Speedbird"
        void setTelephonyDesignator(const QString &telephony) { m_telephonyDesignator = telephony.trimmed().toUpper(); }

        //! Group designator
        const QString &getGroupDesignator() const { return m_groupDesignator; }

        //! Group designator
        void setGroupDesignator(const QString &designator) { m_groupDesignator = designator.trimmed().toUpper(); }

        //! Group name
        const QString &getGroupName() const { return m_groupName; }

        //! Group name
        void setGroupName(const QString &name) { m_groupName = name.trimmed(); }

        //! Group id
        int getGroupId() const { return m_groupId; }

        //! Group id
        void setGroupId(int id) { m_groupId = id; }

        //! Are we a member of a group?
        bool hasGroupMembership() const { return m_groupId >= 0 && !m_groupDesignator.isEmpty(); }

        //! Virtual airline
        bool isVirtualAirline() const { return m_isVa; }

        //! Virtual airline
        void setVirtualAirline(bool va) { m_isVa = va; }

        //! Operating?
        bool isOperating() const { return m_isOperating; }

        //! Operating airline?
        void setOperating(bool operating) { m_isOperating = operating; }

        //! Military, air force or such?
        bool isMilitary() const { return m_isMilitary; }

        //! Military, air force or such?
        void setMilitary(bool military) { m_isMilitary = military; }

        //! Country?
        bool hasValidCountry() const;

        //! Airline designator available?
        bool hasValidDesignator() const;

        //! IATA code?
        bool hasIataCode() const;

        //! Matches designator string?
        bool matchesDesignator(const QString &designator) const;

        //! Matches v-designator string?
        bool matchesVDesignator(const QString &designator) const;

        //! Matches IATA code?
        bool matchesIataCode(const QString &iata) const;

        //! Matches IATA code or designator?
        bool matchesDesignatorOrIataCode(const QString &candidate) const;

        //! Matches IATA code or v-designator?
        bool matchesVDesignatorOrIataCode(const QString &candidate) const;

        //! Matches telephony designator (aka callsign, not to be confused with CCallsign)
        bool matchesTelephonyDesignator(const QString &candidate) const;

        //! Relaxed check by name or telephony designator (aka callsign, not to be confused with CCallsign)
        bool matchesNamesOrTelephonyDesignator(const QString &candidate) const;

        //! Does simplified name contain the candidate
        bool isContainedInSimplifiedName(const QString &candidate) const;

        //! Telephony designator?
        bool hasTelephonyDesignator() const { return !m_telephonyDesignator.isEmpty(); }

        //! Has (airline) name?
        bool hasName() const { return !m_name.isEmpty(); }

        //! Has simplified airline name?
        bool hasSimplifiedName() const;

        //! Complete data
        bool hasCompleteData() const;

        //! Comined string with key
        QString getCombinedStringWithKey() const;

        //! What is better, the callsign airline code or this code. Return the better one.
        CAirlineIcaoCode thisOrCallsignCode(const CCallsign &callsign) const;

        //! \copydoc Mixin::Icon::toIcon
        CIcons::IconIndex toIcon() const;

        //! \copydoc Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAirlineIcaoCode &compareValue) const;

        //! Get icon resource path
        QString getIconResourcePath() const;

        //! Icon has HTML img
        QString getIconAsHTMLImage() const;

        //! Validate data
        CStatusMessageList validate() const;

        //! Update missing parts
        void updateMissingParts(const CAirlineIcaoCode &otherIcaoCode);

        //! As a brief HTML summary (e.g. used in tooltips)
        QString asHtmlSummary() const;

        //! Score against other code 0..100
        int calculateScore(const CAirlineIcaoCode &otherCode, CStatusMessageList *log = nullptr) const;

        //! Null ICAO?
        bool isNull() const;

        //! NULL object
        static const CAirlineIcaoCode &null();

        //! Valid designator?
        static bool isValidAirlineDesignator(const QString &airline);

        //! Valid IATA code?
        static bool isValidIataCode(const QString &iataCode);

        //! Some special valid designator which do not fit standard rule (e.g. 3-letter code)
        static QSet<QString> specialValidDesignators();

        //! Normalize string as airline designator
        static QString normalizeDesignator(const QString &candidate);

        //! Specialized log message for matching / reverse lookup
        //! \threadsafe
        static CStatusMessage logMessage(
            const CAirlineIcaoCode &icaoCode,
            const QString &message, const QStringList &extraCategories = {},
            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        //! Specialized log for matching / reverse lookup
        //! \threadsafe
        static void addLogDetailsToList(CStatusMessageList *log, const CAirlineIcaoCode &icao,
                                        const QString &message, const QStringList &extraCategories = {},
                                        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo);

        //! From our DB JSON
        static CAirlineIcaoCode fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! The airline icon names (name only, not path)
        static const QSet<int> &iconIds();

    private:
        QString m_designator; //!< "DLH"
        QString m_iataCode; //!< "LH"
        QString m_name; //!< "Lufthansa"
        QString m_telephonyDesignator; //!< "Speedbird"
        QString m_groupDesignator; //!< Group designator
        QString m_groupName; //!< Group name
        CCountry m_country; //!< Country
        int m_groupId = -1; //!< Group id
        bool m_isVa = false; //!< virtual airline
        bool m_isOperating = true; //!< still operating?
        bool m_isMilitary = false; //!< Air Force or such

        BLACK_METACLASS(
            CAirlineIcaoCode,
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch),
            BLACK_METAMEMBER(designator),
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(country),
            BLACK_METAMEMBER(telephonyDesignator),
            BLACK_METAMEMBER(groupDesignator),
            BLACK_METAMEMBER(groupName),
            BLACK_METAMEMBER(groupId),
            BLACK_METAMEMBER(isVa),
            BLACK_METAMEMBER(isOperating),
            BLACK_METAMEMBER(isMilitary)
        );
    };

    //! Airline ICAO/id list
    using AirlineIcaoIdMap = QMap<int, CAirlineIcaoCode>;

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirlineIcaoCode)

#endif // guard
