/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRLINEICAOCODE_H
#define BLACKMISC_AVIATION_AIRLINEICAOCODE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/country.h"
#include "blackmisc/datastore.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for ICAO classification
        class BLACKMISC_EXPORT CAirlineIcaoCode :
            public CValueObject<CAirlineIcaoCode>,
            public BlackMisc::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexAirlineDesignator = BlackMisc::CPropertyIndex::GlobalIndexCAirlineIcaoCode,
                IndexIataCode,
                IndexAirlineName,
                IndexAirlineCountryIso,
                IndexAirlineCountry,
                IndexTelephonyDesignator,
                IndexIsVirtualAirline,
                IndexIsOperating,
                IndexDesignatorNameCountry,
            };

            //! Default constructor.
            CAirlineIcaoCode() = default;

            //! Constructor.
            CAirlineIcaoCode(const QString &airlineDesignator);

            //! Constructor.
            CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const BlackMisc::CCountry &country, const QString &telephony, bool virtualAirline, bool operating);

            //! Get airline, e.g. "DLH"
            const QString &getDesignator() const { return this->m_designator; }

            //! Get airline, e.g. "DLH", but "VMVA" for virtual airlines
            const QString getVDesignator() const;

            //! Set airline, e.g. "DLH"
            void setDesignator(const QString &icaoDesignator);

            //! IATA code
            const QString &getIataCode() const { return m_iataCode; }

            //! Set IATA code
            void setIataCode(const QString &iataCode) { this->m_iataCode = iataCode.trimmed().toUpper(); }

            //! Get country, e.g. "FR"
            const QString &getCountryIso() const { return this->m_country.getIsoCode(); }

            //! Get country, e.g. "FRANCE"
            const BlackMisc::CCountry &getCountry() const { return this->m_country; }

            //! Combined string designator, name, country
            QString getDesignatorNameCountry() const;

            //! Set country
            void setCountry(const BlackMisc::CCountry &country) { this->m_country = country; }

            //! Get name, e.g. "Lufthansa"
            const QString &getName() const { return this->m_name; }

            //! Name plus key, e.g. "Lufthansa (3421)"
            QString getNameWithKey() const;

            //! Set name
            void setName(const QString &name) { this->m_name = name.trimmed(); }

            //! Telephony designator such as "Speedbird"
            const QString &getTelephonyDesignator() const { return this->m_telephonyDesignator; }

            //! Telephony designator such as "Speedbird"
            void setTelephonyDesignator(const QString &telephony) { this->m_telephonyDesignator = telephony.trimmed().toUpper(); }

            //! Virtual airline
            bool isVirtualAirline() const { return m_isVa; }

            //! Virtual airline
            void setVirtualAirline(bool va) { m_isVa = va; }

            //! Operating?
            bool isOperating() const { return m_isOperating; }

            //! Operating airline?
            void setOperating(bool operating) { m_isOperating = operating; }

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

            //! Telephony designator?
            bool hasTelephonyDesignator() const { return !this->m_telephonyDesignator.isEmpty(); }

            //! Has (airline) name?
            bool hasName() const { return !m_name.isEmpty(); }

            //! Complete data
            bool hasCompleteData() const;

            //! Comined string with key
            QString getCombinedStringWithKey() const;

            //! \copydoc CValueObject::toIcon
            CIcon toIcon() const;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Compare for index
            int comparePropertyByIndex(const CAirlineIcaoCode &compareValue, const CPropertyIndex &index) const;

            //! Validate data
            BlackMisc::CStatusMessageList validate() const;

            //! Update missing parts
            void updateMissingParts(const CAirlineIcaoCode &otherIcaoCode);

            //! Valid designator?
            static bool isValidAirlineDesignator(const QString &airline);

            //! From our DB JSON
            static CAirlineIcaoCode fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirlineIcaoCode)
            QString m_designator;           //!< "DLH"
            QString m_iataCode;             //!< "LH"
            QString m_name;                 //!< "Lufthansa"
            BlackMisc::CCountry m_country;  //!< Country
            QString m_telephonyDesignator;  //!< "Speedbird"
            bool m_isVa = false;            //!< virtual airline
            bool m_isOperating = true;      //!< still operating?
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirlineIcaoCode)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirlineIcaoCode, (
                                   o.m_dbKey,
                                   o.m_timestampMSecsSinceEpoch,
                                   o.m_designator,
                                   o.m_name,
                                   o.m_country,
                                   o.m_telephonyDesignator,
                                   o.m_isVa,
                                   o.m_isOperating
                               ))

#endif // guard
