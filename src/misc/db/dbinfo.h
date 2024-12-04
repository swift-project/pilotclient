// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DBINFO_H
#define SWIFT_MISC_DB_DBINFO_H

#include "misc/db/datastore.h"
#include "misc/network/entityflags.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::db, CDbInfo)

namespace swift::misc::db
{
    /*!
     * Info about the latest models
     */
    class SWIFT_MISC_EXPORT CDbInfo : public CValueObject<CDbInfo>, public IDatastoreObjectWithIntegerKey
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
        network::CEntityFlags::Entity getEntity() const;

        //! The shared file name such as "airports.json"
        const QString &getSharedFileName() const;

        //! Service name such as "jsonairport.php"
        const QString &getServiceName() const;

        //! Set entity, should be in sync with a corresponding table name
        void setEntity(network::CEntityFlags::Entity entity);

        //! Entry count
        int getEntries() const { return m_entries; }

        //! Matches given entity
        bool matchesEntity(network::CEntityFlags::Entity entity) const;

        //! Set entries
        void setEntries(int entries) { m_entries = entries; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
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
        static const QString &entityToSharedName(network::CEntityFlags::Entity entity);

        //! Get service file name
        static const QString &entityToServiceName(network::CEntityFlags::Entity entity);

    private:
        QString m_tableName; //!< table name
        int m_entries; //!< number of entries
        network::CEntityFlags::Entity m_entity = network::CEntityFlags::NoEntity; //!< lazy initialized entity

        SWIFT_METACLASS(
            CDbInfo,
            SWIFT_METAMEMBER(dbKey),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
            SWIFT_METAMEMBER(tableName),
            SWIFT_METAMEMBER(entries));
    };
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CDbInfo)

#endif // SWIFT_MISC_DB_DBINFO_H
