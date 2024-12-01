// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DB_DATABASE_READERCONFIG_H
#define SWIFT_CORE_DB_DATABASE_READERCONFIG_H

#include <QDateTime>
#include <QJsonArray>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "core/swiftcoreexport.h"
#include "misc/db/dbflags.h"
#include "misc/network/entityflags.h"
#include "misc/pq/time.h"
#include "misc/sequence.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::core::db, CDatabaseReaderConfig)

namespace swift::core::db
{
    //! Details how to read a certain entity
    class SWIFT_CORE_EXPORT CDatabaseReaderConfig : public swift::misc::CValueObject<CDatabaseReaderConfig>
    {
    public:
        //! Default constructor.
        CDatabaseReaderConfig() = default;

        //! Constructor for one or multiple entities
        CDatabaseReaderConfig(
            swift::misc::network::CEntityFlags::Entity entities,
            swift::misc::db::CDbFlags::DataRetrievalMode retrievalFlags,
            const swift::misc::physical_quantities::CTime &cacheLifetime = swift::misc::physical_quantities::CTime());

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Supported entities
        swift::misc::network::CEntityFlags::Entity getEntities() const;

        //! Supports given entites?
        bool supportsEntities(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Supported modes
        swift::misc::db::CDbFlags::DataRetrievalMode getRetrievalMode() const;

        //! DB is down
        void markAsDbDown();

        //! Timeout
        void setCacheLifetime(const swift::misc::physical_quantities::CTime &time);

        //! Will read from swift DB
        bool possiblyReadsFromSwiftDb() const;

        //! Needs the DB info file loaded
        //! \remark a possible counterpart for the DB info file is possiblyReadsFromSwiftDb
        bool needsSharedInfoFile() const;

        //! Will write to swift DB
        bool possiblyWritesToSwiftDb() const;

        //! Will read from cache
        bool possiblyReadsFromCache() const;

        //! Fully initialized
        bool isValid() const;

    private:
        int m_entities = swift::misc::network::CEntityFlags::NoEntity; //!< swift::misc::network::CEntityFlags::Entity
        int m_retrievalMode = swift::misc::db::CDbFlags::DbReading; //!< swift::misc::db::CDbFlags::DataRetrievalMode
        swift::misc::physical_quantities::CTime m_cacheLifetime;

        SWIFT_METACLASS(
            CDatabaseReaderConfig,
            SWIFT_METAMEMBER(entities),
            SWIFT_METAMEMBER(retrievalMode),
            SWIFT_METAMEMBER(cacheLifetime));
    };
} // namespace swift::core::db

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::core::db, CDatabaseReaderConfig, CDatabaseReaderConfigList)

namespace swift::core::db
{
    //! Value object encapsulating a list of reader configs.
    class SWIFT_CORE_EXPORT CDatabaseReaderConfigList :
        public swift::misc::CSequence<CDatabaseReaderConfig>,
        public swift::misc::mixin::MetaType<CDatabaseReaderConfigList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CDatabaseReaderConfigList)
        using CSequence::CSequence;

        //! Default constructor.
        CDatabaseReaderConfigList() = default;

        //! Construct from a base class object.
        CDatabaseReaderConfigList(const CSequence<CDatabaseReaderConfig> &other);

        //! Find first one matching given
        //! \remark works for single and multiple entities
        CDatabaseReaderConfig
        findFirstOrDefaultForEntity(const swift::misc::network::CEntityFlags::Entity entities) const;

        //! DB is down
        void markAsDbDown();

        //! Update lifetimes
        void setCacheLifetimes(const swift::misc::physical_quantities::CTime &time);

        //! Will read from swift DB
        bool possiblyReadsFromSwiftDb() const;

        //! Will write to swift DB
        bool possiblyWritesToSwiftDb() const;

        //! Needs any shared info object
        bool needsSharedInfoObjects(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Needs any shared info object, but only if the cache is empty
        //! \remark needs readers initialized
        bool needsSharedInfoObjectsIfCachesEmpty(swift::misc::network::CEntityFlags::Entity entities,
                                                 swift::misc::network::CEntityFlags::Entity cachedEntities) const;

        //! Needs any shared header loaded before continued
        bool needsSharedInfoFile(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Entities which will use cache or DB, so no canceled or ignored ones
        swift::misc::network::CEntityFlags::Entity getEntitesCachedOrReadFromDB() const;

        //! Init for mapping tool
        static CDatabaseReaderConfigList forMappingTool();

        //! Init for pilot client
        static CDatabaseReaderConfigList forPilotClient();

        //! Init for launcher
        static CDatabaseReaderConfigList forLauncher();

        //! Init all with direct DB access
        static CDatabaseReaderConfigList allDirectDbAccess();
    };
} // namespace swift::core::db

Q_DECLARE_METATYPE(swift::core::db::CDatabaseReaderConfig)
Q_DECLARE_METATYPE(swift::core::db::CDatabaseReaderConfigList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::core::db::CDatabaseReaderConfig>)

#endif // SWIFT_CORE_DB_DATABASE_READERCONFIG_H
