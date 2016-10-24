/* Copyright (C) 2016
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

        public:
            //! Constructor
            CAirportDataReader(QObject* parent, const CDatabaseReaderConfigList &config);

            //! Returns a list of all airports in the database.
            //! \threadsafe
            BlackMisc::Aviation::CAirportList getAirports() const;

            // base class overrides
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const override;
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;

        protected:
            // base class overrides
            virtual void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const override;

        private:
            //! URL for airport list
            BlackMisc::Network::CUrl getAirportsUrl() const;

        private slots:
            //! Parse downloaded JSON file
            void ps_parseAirportData(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read(BlackMisc::Network::CEntityFlags::Entity entity = BlackMisc::Network::CEntityFlags::DistributorLiveryModel, const QDateTime &newerThan = QDateTime());

            //! Airport cache changed
            void ps_airportCacheChanged();

            //! Base url cache changed
            void ps_baseUrlCacheChanged();

        private:
            BlackMisc::CData<BlackCore::Data::TDbAirportCache> m_airportCache {this, &CAirportDataReader::ps_airportCacheChanged};

            //! Reader URL (we read from where?) used to detect changes of location
            BlackMisc::CData<BlackCore::Data::TDbModelReaderBaseUrl> m_readerUrlCache {this, &CAirportDataReader::ps_baseUrlCacheChanged };

            //! Update reader URL
            void updateReaderUrl(const BlackMisc::Network::CUrl &url);

            //! Base URL
            //! \threadsafe
            static const BlackMisc::Network::CUrl &getBaseUrl();
        };
    }
} // ns

#endif // guard
