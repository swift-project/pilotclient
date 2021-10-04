/* Copyright (C) 2016
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DBINFO_H
#define BLACKMISC_DB_DBINFO_H

#include "blackmisc/db/datastore.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Db, CDbInfo)

namespace BlackMisc::Db
{
    /*!
     * Info about the latest models
     */
    class BLACKMISC_EXPORT CDbInfo :
        public CValueObject<CDbInfo>,
        public IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexTableName = CPropertyIndexRef::GlobalIndexCDbInfo,
            IndexEntries,
            IndexEntity
        };

        //! Constructor
        CDbInfo() = default;

        //! Constructor
        CDbInfo(int key, const QString &tableName, int entries);

        //! Valid?
        bool isValid() const;

        //! Table name
        const QString &getTableName() const { return m_tableName; }

        //! Set table  name
        void setTableName(const QString &tableName);

        //! Get entity (based on table name
        Network::CEntityFlags::Entity getEntity() const;

        //! The shared file name such as "airports.json"
        const QString &getSharedFileName() const;

        //! Service name such as "jsonairport.php"
        const QString &getServiceName() const;

        //! Set entity, should be in sync with a corresponding table name
        void setEntity(Network::CEntityFlags::Entity entity);

        //! Entry count
        int getEntries() const { return m_entries; }

        //! Matches given entity
        bool matchesEntity(Network::CEntityFlags::Entity entity) const;

        //! Set entries
        void setEntries(int entries) { m_entries = entries; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Compare by index
        int comparePropertyByIndex(CPropertyIndexRef index, const CDbInfo &compareValue) const;

        //! From our database JSON format
        static CDbInfo fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

        //! The shared file names
        static const QStringList &sharedFileNames();

        //! Get shared info file name
        static const QString &sharedInfoFileName();

        //! Service names
        static const QStringList &serviceNames();

        //! Get shared file name
        static const QString &entityToSharedName(Network::CEntityFlags::Entity entity);

        //! Get service file name
        static const QString &entityToServiceName(Network::CEntityFlags::Entity entity);

    private:
        QString m_tableName; //!< table name
        int     m_entries;   //!< number of entries
        Network::CEntityFlags::Entity m_entity = Network::CEntityFlags::NoEntity; //!< lazy initialized entity

        BLACK_METACLASS(
            CDbInfo,
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch),
            BLACK_METAMEMBER(tableName),
            BLACK_METAMEMBER(entries)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Db::CDbInfo)

#endif // guard
