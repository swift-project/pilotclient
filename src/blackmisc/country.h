/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COUNTRY_H
#define BLACKMISC_COUNTRY_H

#include "blackmisc/db/datastore.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CCountry)

namespace BlackMisc
{
    /*!
     * Color
     */
    class BLACKMISC_EXPORT CCountry :
        public CValueObject<CCountry>,
        public Db::IDatastoreObjectWithStringKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexIsoCode = CPropertyIndexRef::GlobalIndexCCountry,
            IndexIso3Code,
            IndexName,
            IndexAlias1,
            IndexAlias2,
            IndexNameIso,
            IndexIsoName,
            IndexHistoric
        };

        //! Constructor
        CCountry() {}

        //! Constructor
        CCountry(const QString &iso, const QString &name);

        //! Valid?
        bool isValid() const;

        //! Representing icon
        CIcons::IconIndex toIcon() const;

        //! DB ISO code
        const QString &getIsoCode() const { return m_dbKey; }

        //! Get 3 letter iso code
        const QString &getIso3Code() const { return m_iso3; }

        //! Country ISO code (US, DE, GB, PL)
        void setIsoCode(const QString &iso);

        //! Country ISO code (USA, DEU, GBR, POL)
        void setIso3Code(const QString &iso);

        //! ISO code?
        bool hasIsoCode() const;

        //! ISO 3 letter code?
        bool hasIso3Code() const;

        //! Country name
        const QString &getName() const { return m_name; }

        //! Country name (no accents ...)
        const QString &getSimplifiedName() const { return m_simplifiedName; }

        //! Alias 1
        const QString &getAlias1() const { return m_alias1; }

        //! Alias 1
        void setAlias1(const QString &alias);

        //! Alias 2
        const QString &getAlias2() const { return m_alias2; }

        //! Alias 2
        void setAlias2(const QString &alias);

        //! Historic / non-existing country (e.g. Soviet Union)
        bool isHistoric() const { return m_historic; }

        //! Historic country?
        void setHistoric(bool historic) { m_historic = historic; }

        //! Combined string ISO/name
        QString getCombinedStringIsoName() const;

        //! Combined string name/ISO
        QString getCombinedStringNameIso() const;

        //! Set country  name
        void setName(const QString &countryName);

        //! Matches country name
        bool matchesCountryName(const QString &name) const;

        //! Matches alias
        bool matchesAlias(const QString &alias) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(CPropertyIndexRef index, const CCountry &compareValue) const;

        //! From our database JSON format
        static CCountry fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! Valid country iso code
        static bool isValidIsoCode(const QString &isoCode);

    private:
        //! Set a simplified name if not equal to "official name"
        void setSimplifiedNameIfNotSame();

        QString m_iso3;              //!< 3 letter code
        QString m_name;              //!< country name
        QString m_simplifiedName;    //!< no accent characters
        QString m_alias1;            //!< 1st alias
        QString m_alias2;            //!< 2nd alias
        bool    m_historic = false;  //!< historic country

        BLACK_METACLASS(
            CCountry,
            BLACK_METAMEMBER(dbKey, 0, CaseInsensitiveComparison),
            BLACK_METAMEMBER(loadedFromDb),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch),
            BLACK_METAMEMBER(iso3),
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(simplifiedName),
            BLACK_METAMEMBER(alias1),
            BLACK_METAMEMBER(alias2),
            BLACK_METAMEMBER(historic)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CCountry)

#endif // guard
