/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_MODELDATAREADER_H
#define BLACKCORE_MODELDATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/dbcaches.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QStringList>

class QNetworkReply;

namespace BlackCore
{
    namespace Db
    {
        //! Read model related data from Database
        class BLACKCORE_EXPORT CModelDataReader : public CDatabaseReader
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

            //! Get aircraft liveries
            //! \threadsafe
            BlackMisc::Aviation::CLiveryList getLiveries() const;

            //! Get aircraft livery for code
            //! \threadsafe
            BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

            //! Get aircraft livery for ICAO code
            //! \threadsafe
            BlackMisc::Aviation::CLivery getStdLiveryForAirlineVDesignator(const BlackMisc::Aviation::CAirlineIcaoCode &icao) const;

            //! Get aircraft livery for id
            //! \threadsafe
            BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const;

            //! Best match specified by livery
            //! \threadsafe
            BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const;

            //! Get distributors (of models)
            //! \threadsafe
            BlackMisc::Simulation::CDistributorList getDistributors() const;

            //! Get distributor for id
            //! \threadsafe
            BlackMisc::Simulation::CDistributor getDistributorForDbKey(const QString &dbKey) const;

            //! Get models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getModels() const;

            //! Get model for string
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

            //! Get model for DB key
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModel getModelForDbKey(int dbKey) const;

            //! Get model for designator/combined code
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode);

            //! Get aircraft liveries count
            //! \threadsafe
            int getLiveriesCount() const;

            //! Get model distributors count
            //! \threadsafe
            int getDistributorsCount() const;

            //! Best match specified by distributor
            //! \threadsafe
            BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributorPattern) const;

            //! Best match specified by distributor
            //! \threadsafe
            BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributorPattern, const BlackMisc::Simulation::CAircraftModel &model) const;

            //! Get models count
            //! \threadsafe
            int getModelsCount() const;

            //! Get model keys
            //! \threadsafe
            QSet<int> getModelDbKeys() const;

            //! Get model keys
            //! \threadsafe
            QStringList getModelStringList() const;

            //! All data read?
            //! \threadsafe
            bool areAllDataRead() const;

            //! Read from local JSON file
            BlackMisc::CStatusMessageList readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

            //! Read from static DB data file
            bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

            //! Write to JSON file
            bool writeToJsonFiles(const QString &dir) const;

            // cache handling for base class
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

        private slots:
            //! Liveries have been read
            void ps_parseLiveryData(QNetworkReply *nwReply);

            //! Distributors have been read
            void ps_parseDistributorData(QNetworkReply *nwReply);

            //! Models have been read
            void ps_parseModelData(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::DistributorLiveryModel,
                         BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode = BlackMisc::Db::CDbFlags::DbReading,
                         const QDateTime &newerThan = QDateTime());

            void ps_liveryCacheChanged();
            void ps_modelCacheChanged();
            void ps_distributorCacheChanged();
            void ps_baseUrlCacheChanged();

        private:
            BlackMisc::CData<BlackCore::Data::TDbLiveryCache>      m_liveryCache {this, &CModelDataReader::ps_liveryCacheChanged };
            BlackMisc::CData<BlackCore::Data::TDbModelCache>       m_modelCache  {this, &CModelDataReader::ps_modelCacheChanged };
            BlackMisc::CData<BlackCore::Data::TDbDistributorCache> m_distributorCache {this, &CModelDataReader::ps_distributorCacheChanged };

            //! Reader URL (we read from where?) used to detect changes of location
            BlackMisc::CData<BlackCore::Data::TDbModelReaderBaseUrl> m_readerUrlCache {this, &CModelDataReader::ps_baseUrlCacheChanged };

            //! Update reader URL
            void updateReaderUrl(const BlackMisc::Network::CUrl &url);

            //! URL livery web service
            BlackMisc::Network::CUrl getLiveryUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

            //! URL distributor web service
            BlackMisc::Network::CUrl getDistributorUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

            //! URL model web service
            BlackMisc::Network::CUrl getModelUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;
        };
    } // ns
} // ns

#endif // guard
