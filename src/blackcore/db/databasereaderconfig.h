/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_DATABASE_READERCONFIG_H
#define BLACKCORE_DB_DATABASE_READERCONFIG_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/db/dbflags.h"
#include "blackmisc/sequence.h"
#include "blackmisc/valueobject.h"

#include <QDateTime>
#include <QJsonArray>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QtGlobal>

namespace BlackCore
{
    namespace Db
    {
        //! Details how to read a certain entity
        class BLACKCORE_EXPORT CDatabaseReaderConfig : public BlackMisc::CValueObject<CDatabaseReaderConfig>
        {
        public:
            //! Default constructor.
            CDatabaseReaderConfig() = default;

            //! Constructor for one or multiple entities
            CDatabaseReaderConfig(BlackMisc::Network::CEntityFlags::Entity entities,
                                  BlackMisc::Db::CDbFlags::DataRetrievalMode retrievalFlags,
                                  const BlackMisc::PhysicalQuantities::CTime &cacheLifetime = BlackMisc::PhysicalQuantities::CTime());

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Supported entities
            BlackMisc::Network::CEntityFlags::Entity getEntities() const;

            //! Supports given entites?
            bool supportsEntities(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Supported modes
            BlackMisc::Db::CDbFlags::DataRetrievalMode getRetrievalMode() const;

            //! DB is down
            void markAsDbDown();

            //! Timeout
            void setCacheLifetime(const BlackMisc::PhysicalQuantities::CTime &time);

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
            int m_entities = BlackMisc::Network::CEntityFlags::NoEntity; //!< BlackMisc::Network::CEntityFlags::Entity
            int m_retrievalMode = BlackMisc::Db::CDbFlags::DbReading;   //!< BlackMisc::Db::CDbFlags::DataRetrievalMode
            BlackMisc::PhysicalQuantities::CTime m_cacheLifetime;

            BLACK_METACLASS(
                CDatabaseReaderConfig,
                BLACK_METAMEMBER(entities),
                BLACK_METAMEMBER(retrievalMode),
                BLACK_METAMEMBER(cacheLifetime));
        };

        //! Value object encapsulating a list of reader configs.
        class BLACKCORE_EXPORT CDatabaseReaderConfigList :
            public BlackMisc::CSequence<CDatabaseReaderConfig>,
            public BlackMisc::Mixin::MetaType<CDatabaseReaderConfigList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDatabaseReaderConfigList)
            using CSequence::CSequence;

            //! Default constructor.
            CDatabaseReaderConfigList() = default;

            //! Construct from a base class object.
            CDatabaseReaderConfigList(const CSequence<CDatabaseReaderConfig> &other);

            //! Find first one matching given
            //! \remark works for single and multiple entities
            CDatabaseReaderConfig findFirstOrDefaultForEntity(const BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! DB is down
            void markAsDbDown();

            //! Update lifetimes
            void setCacheLifetimes(const BlackMisc::PhysicalQuantities::CTime &time);

            //! Will read from swift DB
            bool possiblyReadsFromSwiftDb() const;

            //! Will write to swift DB
            bool possiblyWritesToSwiftDb() const;

            //! Needs any shared info object
            bool needsSharedInfoObjects(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Needs any shared info object, but only if the cache is empty
            //! \remark needs readers initialized
            bool needsSharedInfoObjectsIfCachesEmpty(
                BlackMisc::Network::CEntityFlags::Entity entities,
                BlackMisc::Network::CEntityFlags::Entity cachedEntities) const;

            //! Needs any shared header loaded before continued
            bool needsSharedInfoFile(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Entities which will use cache or DB, so no canceled or ignored ones
            BlackMisc::Network::CEntityFlags::Entity getEntitesCachedOrReadFromDB() const;

            //! Init for mapping tool
            static CDatabaseReaderConfigList forMappingTool();

            //! Init for pilot client
            static CDatabaseReaderConfigList forPilotClient();

            //! Init for launcher
            static CDatabaseReaderConfigList forLauncher();

            //! Init all with direct DB access
            static CDatabaseReaderConfigList allDirectDbAccess();
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Db::CDatabaseReaderConfig)
Q_DECLARE_METATYPE(BlackCore::Db::CDatabaseReaderConfigList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackCore::Db::CDatabaseReaderConfig>)

#endif // guard
