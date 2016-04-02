/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COUNTRY_H
#define BLACKMISC_COUNTRY_H

#include "blackmiscexport.h"
#include "datastore.h"
#include "valueobject.h"
#include <QColor>

namespace BlackMisc
{
    /*!
     * Color
     */
    class BLACKMISC_EXPORT CCountry :
        public CValueObject<CCountry>,
        public IDatastoreObjectWithStringKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexIsoCode = BlackMisc::CPropertyIndex::GlobalIndexCCountry,
            IndexName,
            IndexNameIso,
            IndexIsoName
        };

        //! Constructor
        CCountry() = default;

        //! Constructor
        CCountry(const QString &iso, const QString &name);

        //! Valid?
        bool isValid() const;

        //! Representing icon
        CIcon toIcon() const;

        //! DB ISO code
        const QString &getIsoCode() const { return m_dbKey; }

        //! Country ISO code (US, DE, GB, PL)
        void setIsoCode(const QString &iso);

        //! ISO code?
        bool hasIsoCode() const;

        //! Country name
        const QString &getName() const { return m_name; }

        //! Combined string ISO/name
        QString getCombinedStringIsoName() const;

        //! Combined string name/ISO
        QString getCombinedStringNameIso() const;

        //! Set country  name
        void setName(const QString &countryName);

        //! Matches country name
        bool matchesCountryName(const QString &name) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

        //! Compare for index
        int comparePropertyByIndex(const CCountry &compareValue, const CPropertyIndex &index) const;

        //! From our database JSON format
        static CCountry fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! Valid country iso code
        static bool isValidIsoCode(const QString &isoCode);

    private:
        QString m_name; //!< country name

        BLACK_METACLASS(CCountry,
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch),
            BLACK_METAMEMBER(name)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CCountry)

#endif // guard
