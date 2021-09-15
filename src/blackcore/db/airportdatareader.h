/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRPORTDATAREADER_H
#define BLACKCORE_AIRPORTDATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/dbcaches.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/entityflags.h"

#include <QNetworkAccessManager>
#include <atomic>

namespace BlackCore::Db
{
    //! Reader for airport database data.
    class BLACKCORE_EXPORT CAirportDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        CAirportDataReader(QObject *parent, const CDatabaseReaderConfigList &config);

        //! Returns a list of all airports in the database.
        //! \threadsafe
        BlackMisc::Aviation::CAirportList getAirports() const;

        //! Returns airport for designator (or default)
        //! \threadsafe
        BlackMisc::Aviation::CAirport getAirportForIcaoDesignator(const QString &designator) const;

        //! Get airports for location
        //! \threadsafe
        BlackMisc::Aviation::CAirport getAirportForNameOrLocation(const QString &location) const;

        //! Returns a list of all airports in the database.
        //! \threadsafe
        int getAirportsCount() const;

        // data read from local data
        virtual BlackMisc::CStatusMessageList readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;

        // base class overrides
        virtual BlackMisc::Network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const override;
        virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

    protected:
        // base class overrides
        virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CUrl &oldUrlInfo, BlackMisc::Network::CUrl &newUrlInfo) const override;
        virtual BlackMisc::Network::CUrl getDbServiceBaseUrl() const override;

    private:
        BlackMisc::CData<BlackCore::Data::TDbAirportCache> m_airportCache {this, &CAirportDataReader::airportCacheChanged}; //!< cache file
        std::atomic_bool m_syncedAirportCache { false }; //!< already synchronized?

        //! Reader URL (we read from where?) used to detect changes of location
        BlackMisc::CData<BlackCore::Data::TDbModelReaderBaseUrl> m_readerUrlCache {this, &CAirportDataReader::baseUrlCacheChanged };

        //! \copydoc CDatabaseReader::read
        void read(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        //! Parse downloaded JSON file
        void parseAirportData(QNetworkReply *nwReplyPtr);

        //! Airport cache changed
        void airportCacheChanged();

        //! Base url cache changed
        void baseUrlCacheChanged();

        //! Update reader URL
        void updateReaderUrl(const BlackMisc::Network::CUrl &url);

        //! URL for airport list
        BlackMisc::Network::CUrl getAirportsUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;
    };
} // ns

#endif // guard
