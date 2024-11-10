// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AIRPORTDATAREADER_H
#define BLACKCORE_AIRPORTDATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/dbcaches.h"
#include "blackcore/db/databasereader.h"
#include "misc/aviation/airportlist.h"
#include "misc/network/entityflags.h"

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
        swift::misc::aviation::CAirportList getAirports() const;

        //! Returns airport for designator (or default)
        //! \threadsafe
        swift::misc::aviation::CAirport getAirportForIcaoDesignator(const QString &designator) const;

        //! Get airports for location
        //! \threadsafe
        swift::misc::aviation::CAirport getAirportForNameOrLocation(const QString &location) const;

        //! Returns a list of all airports in the database.
        //! \threadsafe
        int getAirportsCount() const;

        // data read from local data
        virtual swift::misc::CStatusMessageList readFromJsonFiles(const QString &dir, swift::misc::network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir, swift::misc::network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;

        // base class overrides
        virtual swift::misc::network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(swift::misc::network::CEntityFlags::Entity entities) const override;
        virtual int getCacheCount(swift::misc::network::CEntityFlags::Entity entity) const override;
        virtual swift::misc::network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual swift::misc::network::CEntityFlags::Entity getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(swift::misc::network::CEntityFlags::Entity entities) override;

    protected:
        // base class overrides
        virtual void invalidateCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CUrl &oldUrlInfo, swift::misc::network::CUrl &newUrlInfo) const override;
        virtual swift::misc::network::CUrl getDbServiceBaseUrl() const override;

    private:
        swift::misc::CData<BlackCore::Data::TDbAirportCache> m_airportCache { this, &CAirportDataReader::airportCacheChanged }; //!< cache file
        std::atomic_bool m_syncedAirportCache { false }; //!< already synchronized?

        //! Reader URL (we read from where?) used to detect changes of location
        swift::misc::CData<BlackCore::Data::TDbModelReaderBaseUrl> m_readerUrlCache { this, &CAirportDataReader::baseUrlCacheChanged };

        //! \copydoc CDatabaseReader::read
        void read(swift::misc::network::CEntityFlags::Entity entity, swift::misc::db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        //! Parse downloaded JSON file
        void parseAirportData(QNetworkReply *nwReplyPtr);

        //! Airport cache changed
        void airportCacheChanged();

        //! Base url cache changed
        void baseUrlCacheChanged();

        //! Update reader URL
        void updateReaderUrl(const swift::misc::network::CUrl &url);

        //! URL for airport list
        swift::misc::network::CUrl getAirportsUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;
    };
} // ns

#endif // guard
