/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

namespace BlackCore
{
    namespace Db
    {
        /**
         * Reader for airport database.
         */
        class BLACKCORE_EXPORT CAirportDataReader : public CDatabaseReader
        {
            Q_OBJECT

        signals:
            //! Emitted when data is parsed
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        public:
            //! Constructor
            CAirportDataReader(QObject* parent, const CDatabaseReaderConfigList &config);

            //! Read airports
            void readInBackgroundThread();

            //! Returns a list of all airports in the database.
            //! \threadsafe
            BlackMisc::Aviation::CAirportList getAirports() const;

            //! \copydoc BlackCore::Db::CDatabaseReader::getCacheTimestamp()
            QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const override;

            //! \copydoc BlackCore::Db::CDatabaseReader:;getCacheCount()
            int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;

        protected:
            //! \copydoc BlackCore::Db::CDatabaseReader::syncronizeCaches()
            void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

            //! \copydoc BlackCore::Db::CDatabaseReader::invalidateCaches()
            void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

            //! \copydoc BlackCore::Db::CDatabaseReader::hasChangedUrl()
            bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const override;

        private:
            //! URL for airport list
            BlackMisc::Network::CUrl getAirportsUrl() const;

        private slots:
            //! Parse downloaded JSON file
            void ps_parseAirportData(QNetworkReply *nwReply);

            //! Read Last-Modified header
            void ps_parseAirportHeader(QNetworkReply *nwReply);

            //! Read airports
            void ps_readAirports();

            //! Airport cache changed
            void ps_airportCacheChanged();

        private:
            BlackMisc::CData<BlackCore::Data::TDbAirportCache> m_airportCache {this, &CAirportDataReader::ps_airportCacheChanged};
            mutable QReadWriteLock m_lock;
            quint64 m_lastModified = 0; //!< When was data file updated, obtained from HTTP Last-Modified header, in ms from epoch

        };
    }
} // ns

#endif // guard
