// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DB_INFODATAREADER_H
#define SWIFT_CORE_DB_INFODATAREADER_H

#include <QNetworkReply>
#include <QObject>
#include <QReadWriteLock>

#include "core/db/databasereader.h"
#include "core/swiftcoreexport.h"
#include "misc/db/dbinfolist.h"

namespace swift::core::db
{
    //! Read information about data from Database or shared files
    //! such as when updated and number of entries.
    //! \see swift::misc::db::CDbInfo
    class SWIFT_CORE_EXPORT CInfoDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config,
                                 swift::misc::db::CDbFlags::DataRetrievalModeFlag mode);

        //! Get info list (either shared or from DB)
        //! \threadsafe
        swift::misc::db::CDbInfoList getInfoObjects() const;

        //! Get info list size (either shared or from DB)
        //! \threadsafe
        int getInfoObjectCount() const;

        //! All data read?
        //! \threadsafe
        bool areAllInfoObjectsRead() const;

        //! Allow to call directly, special for info objects reader
        void readInfoData();

        //! URL depending on mode, i.e. shared/DB
        swift::misc::network::CUrl getInfoObjectsUrl() const;

        // data read from local data
        virtual swift::misc::CStatusMessageList readFromJsonFiles(const QString &dir,
                                                                  swift::misc::network::CEntityFlags::Entity whatToRead,
                                                                  bool overrideNewer) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir,
                                                   swift::misc::network::CEntityFlags::Entity whatToRead,
                                                   bool overrideNewer) override;

        // cache handling for base class: no cache handling here in that case
        virtual swift::misc::network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const override;
        virtual int getCacheCount(swift::misc::network::CEntityFlags::Entity entity) const override;
        virtual swift::misc::network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual swift::misc::network::CEntityFlags::Entity
        getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(swift::misc::network::CEntityFlags::Entity entities) override;

    protected:
        // cache handling for base class
        virtual void invalidateCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(swift::misc::network::CEntityFlags::Entity entity,
                                   swift::misc::network::CUrl &oldUrlInfo,
                                   swift::misc::network::CUrl &newUrlInfo) const override;
        virtual swift::misc::network::CUrl getDbServiceBaseUrl() const override;

    private:
        //! Info objects have been read
        void parseInfoObjectsData(QNetworkReply *nwReply);

        //! URL info objects web service
        swift::misc::network::CUrl getDbInfoObjectsUrl() const;

        //! URL shared info objects
        swift::misc::network::CUrl getSharedInfoObjectsUrl() const;

        //! Info object entity for mode
        //! \remark CEntityFlags::DbInfoObjectEntity or CEntityFlags::SharedInfoObjectEntity
        swift::misc::network::CEntityFlags::EntityFlag getEntityForMode() const;

        //! \copydoc CDatabaseReader::read
        virtual void read(swift::misc::network::CEntityFlags::Entity entities,
                          swift::misc::db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        swift::misc::db::CDbFlags::DataRetrievalModeFlag m_mode; //!< shared or DB web service?
        swift::misc::db::CDbInfoList m_infoObjects;
        swift::misc::network::CUrl m_urlInfoObjects;
        mutable QReadWriteLock m_lockInfoObjects;
    };
} // namespace swift::core::db
#endif // SWIFT_CORE_DB_INFODATAREADER_H
