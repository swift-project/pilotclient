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
#include "blackmisc/datastore.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
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
                IndexAirlineName,
                IndexAirlineCountryIso,
                IndexAirlineCountry,
                IndexTelephonyDesignator,
                IndexIsVirtualAirline
            };

            //! Default constructor.
            CAirlineIcaoCode() = default;

            //! Constructor.
            CAirlineIcaoCode(const QString &airlineDesignator);

            //! Constructor.
            CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const QString &countryIso, const QString &country, const QString &telephony, bool virtualAirline);

            //! Get airline, e.g. "DLH"
            const QString &getDesignator() const { return this->m_designator; }

            //! Get airline, e.g. "DLH", but "VMVA" for virtual airlines
            const QString getVDesignator() const;

            //! Set airline, e.g. "DLH"
            void setDesignator(const QString &icaoDesignator) { this->m_designator = icaoDesignator.trimmed().toUpper(); }

            //! Get country, e.g. "FR"
            const QString &getCountryIso() const { return this->m_countryIso; }

            //! Get country, e.g. "FRANCE"
            const QString &getCountry() const { return this->m_country; }

            //! Set country ISO
            void setCountryIso(const QString &country) { this->m_countryIso = country.trimmed().toUpper(); }

            //! Set country
            void setCountry(const QString &country) { this->m_country = country.trimmed(); }

            //! Get name, e.g. "Lufthansa"
            const QString &getName() const { return this->m_name; }

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

            //! Country?
            bool hasCountryIso() const { return !this->m_countryIso.isEmpty(); }

            //! Airline available?
            bool hasDesignator() const { return !this->m_designator.isEmpty(); }

            //! Telephony designator?
            bool hasTelephonyDesignator() const { return !this->m_telephonyDesignator.isEmpty(); }

            //! Has (airline) name?
            bool hasName() const { return !m_name.isEmpty(); }

            //! Complete data
            bool hasCompleteData() const;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Valid designator?
            static bool isValidAirlineDesignator(const QString &airline);

            //! From our DB JSON
            static CAirlineIcaoCode fromDatabaseJson(const QJsonObject &json);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirlineIcaoCode)
            QString m_designator;           //!< "DLH"
            QString m_name;                 //!< "Lufthansa"
            QString m_countryIso;           //!< "FR"
            QString m_country;              //!< clear text, "Germany"
            QString m_telephonyDesignator;  //!< "Speedbird"
            bool m_isVa = false;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirlineIcaoCode)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirlineIcaoCode, (
                                   o.m_dbKey,
                                   o.m_designator,
                                   o.m_name,
                                   o.m_countryIso,
                                   o.m_country,
                                   o.m_telephonyDesignator,
                                   o.m_isVa
                               ))

#endif // guard
