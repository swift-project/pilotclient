// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DB_INFODATAREADER_H
#define BLACKCORE_DB_INFODATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/db/dbinfolist.h"

#include <QObject>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore::Db
{
    //! Read information about data from Database or shared files
    //! such as when updated and number of entries.
    //! \see BlackMisc::Db::CDbInfo
    class BLACKCORE_EXPORT CInfoDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config, BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode);

        //! Get info list (either shared or from DB)
        //! \threadsafe
        BlackMisc::Db::CDbInfoList getInfoObjects() const;

        //! Get info list size (either shared or from DB)
        //! \threadsafe
        int getInfoObjectCount() const;

        //! All data read?
        //! \threadsafe
        bool areAllInfoObjectsRead() const;

        //! Allow to call directly, special for info objects reader
        void readInfoData();

        //! URL depending on mode, i.e. shared/DB
        BlackMisc::Network::CUrl getInfoObjectsUrl() const;

        // data read from local data
        virtual BlackMisc::CStatusMessageList readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewer) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewer) override;

        // cache handling for base class: no cache handling here in that case
        virtual BlackMisc::Network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const override;
        virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

    protected:
        // cache handling for base class
        virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CUrl &oldUrlInfo, BlackMisc::Network::CUrl &newUrlInfo) const override;
        virtual BlackMisc::Network::CUrl getDbServiceBaseUrl() const override;

    private:
        //! Info objects have been read
        void parseInfoObjectsData(QNetworkReply *nwReply);

        //! URL info objects web service
        BlackMisc::Network::CUrl getDbInfoObjectsUrl() const;

        //! URL shared info objects
        BlackMisc::Network::CUrl getSharedInfoObjectsUrl() const;

        //! Info object entity for mode
        //! \remark CEntityFlags::DbInfoObjectEntity or CEntityFlags::SharedInfoObjectEntity
        BlackMisc::Network::CEntityFlags::EntityFlag getEntityForMode() const;

        //! \copydoc CDatabaseReader::read
        virtual void read(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        BlackMisc::Db::CDbFlags::DataRetrievalModeFlag m_mode; //!< shared or DB web service?
        BlackMisc::Db::CDbInfoList m_infoObjects;
        BlackMisc::Network::CUrl m_urlInfoObjects;
        mutable QReadWriteLock m_lockInfoObjects;
    };
} // ns
#endif // guard
