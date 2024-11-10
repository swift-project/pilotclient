// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COUNTRY_H
#define SWIFT_MISC_COUNTRY_H

#include "misc/db/datastore.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"
#include "misc/swiftmiscexport.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc, CCountry)

namespace swift::misc
{
    /*!
     * Color
     */
    class SWIFT_MISC_EXPORT CCountry :
        public CValueObject<CCountry>,
        public db::IDatastoreObjectWithStringKey
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
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

        QString m_iso3; //!< 3 letter code
        QString m_name; //!< country name
        QString m_simplifiedName; //!< no accent characters
        QString m_alias1; //!< 1st alias
        QString m_alias2; //!< 2nd alias
        bool m_historic = false; //!< historic country

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

Q_DECLARE_METATYPE(swift::misc::CCountry)

#endif // guard
