// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_MODELDATAREADER_H
#define SWIFT_CORE_MODELDATAREADER_H

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>

#include "core/data/dbcaches.h"
#include "core/db/databasereader.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/aircraftcategorylist.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/liverylist.h"
#include "misc/network/entityflags.h"
#include "misc/network/url.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributorlist.h"

class QNetworkReply;

namespace swift::core::db
{
    //! Read model related data from Database
    class SWIFT_CORE_EXPORT CModelDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

        //! Get aircraft liveries
        //! \threadsafe
        swift::misc::aviation::CLiveryList getLiveries() const;

        //! Get aircraft livery for code
        //! \threadsafe
        swift::misc::aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

        //! Get aircraft livery for ICAO code
        //! \threadsafe
        swift::misc::aviation::CLivery
        getStdLiveryForAirlineVDesignator(const swift::misc::aviation::CAirlineIcaoCode &icao) const;

        //! Get aircraft livery for id
        //! \threadsafe
        swift::misc::aviation::CLivery getLiveryForDbKey(int id) const;

        //! Best match specified by livery
        //! \threadsafe
        swift::misc::aviation::CLivery smartLiverySelector(const swift::misc::aviation::CLivery &livery) const;

        //! Get distributors (of models)
        //! \threadsafe
        swift::misc::simulation::CDistributorList getDistributors() const;

        //! Get distributor for id
        //! \threadsafe
        swift::misc::simulation::CDistributor getDistributorForDbKey(const QString &dbKey) const;

        //! Get models
        //! \threadsafe
        swift::misc::simulation::CAircraftModelList getModels() const;

        //! Get model for string
        //! \threadsafe
        swift::misc::simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

        //! Contains modelstring?
        //! \threadsafe
        bool containsModelString(const QString &modelString) const;

        //! Get model for DB key
        //! \threadsafe
        swift::misc::simulation::CAircraftModel getModelForDbKey(int dbKey) const;

        //! Get aircraft ICAO designators (e.g. B737, ..) for given airline
        //! \threadsafe
        QSet<QString> getAircraftDesignatorsForAirline(const swift::misc::aviation::CAirlineIcaoCode &code) const;

        //! Get aircraft ICAO designators (e.g. B737, ..) for given airline
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList
        getAicraftIcaoCodesForAirline(const swift::misc::aviation::CAirlineIcaoCode &code) const;

        //! Get model for designator/combined code
        //! \threadsafe
        swift::misc::simulation::CAircraftModelList
        getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator,
                                                            const QString &combinedCode);

        //! Get aircraft liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Get model distributors count
        //! \threadsafe
        int getDistributorsCount() const;

        //! Best match specified by distributor
        //! \threadsafe
        swift::misc::simulation::CDistributor
        smartDistributorSelector(const swift::misc::simulation::CDistributor &distributorPattern) const;

        //! Best match specified by distributor
        //! \threadsafe
        swift::misc::simulation::CDistributor
        smartDistributorSelector(const swift::misc::simulation::CDistributor &distributorPattern,
                                 const swift::misc::simulation::CAircraftModel &model) const;

        //! Get models count
        //! \threadsafe
        int getModelsCount() const;

        //! Get model keys
        //! \threadsafe
        QSet<int> getModelDbKeys() const;

        //! Get model keys
        //! \threadsafe
        QStringList getModelStringList(bool sort = false) const;

        //! All data read?
        //! \threadsafe
        bool areAllDataRead() const;

        //! Write to JSON file
        bool writeToJsonFiles(const QString &dir);

        // Data read from local data
        virtual swift::misc::CStatusMessageList readFromJsonFiles(const QString &dir,
                                                                  swift::misc::network::CEntityFlags::Entity whatToRead,
                                                                  bool overrideNewerOnly) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir,
                                                   swift::misc::network::CEntityFlags::Entity whatToRead,
                                                   bool overrideNewerOnly) override;

        // cache handling for base class
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
        swift::misc::CData<swift::core::data::TDbLiveryCache> m_liveryCache { this,
                                                                              &CModelDataReader::liveryCacheChanged };
        swift::misc::CData<swift::core::data::TDbModelCache> m_modelCache { this,
                                                                            &CModelDataReader::modelCacheChanged };
        swift::misc::CData<swift::core::data::TDbDistributorCache> m_distributorCache {
            this, &CModelDataReader::distributorCacheChanged
        };
        std::atomic_bool m_syncedLiveryCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedModelCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedDistributorCache { false }; //!< already synchronized?

        //! \copydoc CDatabaseReader::read
        virtual void read(swift::misc::network::CEntityFlags::Entity entities =
                              swift::misc::network::CEntityFlags::DistributorLiveryModel,
                          swift::misc::db::CDbFlags::DataRetrievalModeFlag mode = swift::misc::db::CDbFlags::DbReading,
                          const QDateTime &newerThan = QDateTime()) override;

        //! Reader URL (we read from where?) used to detect changes of location
        swift::misc::CData<swift::core::data::TDbModelReaderBaseUrl> m_readerUrlCache {
            this, &CModelDataReader::baseUrlCacheChanged
        };

        //! Liveries have been read
        void parseLiveryData(QNetworkReply *nwReply);

        //! Distributors have been read
        void parseDistributorData(QNetworkReply *nwReply);

        //! Models have been read
        void parseModelData(QNetworkReply *nwReply);

        //! Livery cache changed elsewhere
        void liveryCacheChanged();

        //! Model cache changed elsewhere
        void modelCacheChanged();

        //! Distributor cache changed elsewhere
        void distributorCacheChanged();

        //! Base URL cache changed
        void baseUrlCacheChanged();

        //! Update reader URL
        void updateReaderUrl(const swift::misc::network::CUrl &url);

        //! Get ICAO codes
        //! \remark cross reader access
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftAircraftIcaos() const;

        //! Get categories
        //! \remark cross reader access
        swift::misc::aviation::CAircraftCategoryList getAircraftCategories() const;

        //! URL livery web service
        swift::misc::network::CUrl getLiveryUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL distributor web service
        swift::misc::network::CUrl getDistributorUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL model web service
        swift::misc::network::CUrl getModelUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;
    };
} // namespace swift::core::db

#endif // guard
