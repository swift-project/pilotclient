/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_DATABASE_READERCONFIG_H
#define BLACKCORE_DB_DATABASE_READERCONFIG_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
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
        //! Details how to read
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

            //! Supported modes
            BlackMisc::Db::CDbFlags::DataRetrievalMode getRetrievalMode() const;

            //! Timeout
            void setCacheLifetime(const BlackMisc::PhysicalQuantities::CTime &time);

            //! Will read from swift DB
            bool possiblyReadsFromSwiftDb() const;

            //! Fully initialized
            bool isValid() const;

        private:
            int m_entities = BlackMisc::Network::CEntityFlags::NoEntity;  //!< BlackMisc::Network::CEntityFlags::Entity
            int m_retrievalFlags = BlackMisc::Db::CDbFlags::DbDirect;     //!< BlackMisc::Db::CDbFlags::DataRetrievalMode
            BlackMisc::PhysicalQuantities::CTime m_cacheLifetime;

            BLACK_METACLASS(
                CDatabaseReaderConfig,
                BLACK_METAMEMBER(entities),
                BLACK_METAMEMBER(retrievalFlags),
                BLACK_METAMEMBER(cacheLifetime));
        };

        //! Value object encapsulating a list of reader configs.
        class BLACKCORE_EXPORT CDatabaseReaderConfigList :
            public BlackMisc::CSequence<CDatabaseReaderConfig>,
            public BlackMisc::Mixin::MetaType<CDatabaseReaderConfigList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDatabaseReaderConfigList)

            //! Default constructor.
            CDatabaseReaderConfigList() = default;

            //! Construct from a base class object.
            CDatabaseReaderConfigList(const CSequence<CDatabaseReaderConfig> &other);

            //! FInd first one matching given
            CDatabaseReaderConfig findFirstOrDefaultForEntity(const BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Update lifetimes
            void setCacheLifetimes(const BlackMisc::PhysicalQuantities::CTime &time);

            //! Will read from swift DB
            bool possiblyReadsFromSwiftDb() const;

            //! Init for mapping tool
            static CDatabaseReaderConfigList forMappingTool();

            //! Init for pilot client
            static CDatabaseReaderConfigList forPilotClient();

            //! Init all with direct DB access
            static CDatabaseReaderConfigList allDirectDbAccess();
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Db::CDatabaseReaderConfig)
Q_DECLARE_METATYPE(BlackCore::Db::CDatabaseReaderConfigList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackCore::Db::CDatabaseReaderConfig>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackCore::Db::CDatabaseReaderConfig>)

#endif // guard
