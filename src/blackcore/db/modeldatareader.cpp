/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/db/modeldatareader.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"

#include <QDir>
#include <QFlags>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QReadLocker>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QTimer>
#include <QUrl>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Db
    {
        CModelDataReader::CModelDataReader(QObject *owner, const CDatabaseReaderConfigList &config) :
            CDatabaseReader(owner, config, "CModelDataReader")
        {
            // init to avoid threading issues
            getBaseUrl();
        }

        CLiveryList CModelDataReader::getLiveries() const
        {
            return this->m_liveryCache.get();
        }

        CLivery CModelDataReader::getLiveryForCombinedCode(const QString &combinedCode) const
        {
            if (!CLivery::isValidCombinedCode(combinedCode)) { return CLivery(); }
            const CLiveryList liveries(getLiveries());
            return liveries.findByCombinedCode(combinedCode);
        }

        CLivery CModelDataReader::getStdLiveryForAirlineVDesignator(const CAirlineIcaoCode &icao) const
        {
            if (!icao.hasValidDesignator()) { return CLivery(); }
            const CLiveryList liveries(getLiveries());
            return liveries.findStdLiveryByAirlineIcaoVDesignator(icao);
        }

        CLivery CModelDataReader::getLiveryForDbKey(int id) const
        {
            if (id < 0) { return CLivery(); }
            const CLiveryList liveries(getLiveries());
            return liveries.findByKey(id);
        }

        CLivery CModelDataReader::smartLiverySelector(const CLivery &liveryPattern) const
        {
            const CLiveryList liveries(getLiveries()); // thread safe copy
            return liveries.smartLiverySelector(liveryPattern);
        }

        CDistributorList CModelDataReader::getDistributors() const
        {
            return m_distributorCache.get();
        }

        CAircraftModelList CModelDataReader::getModels() const
        {
            return m_modelCache.get();
        }

        CAircraftModel CModelDataReader::getModelForModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            const CAircraftModelList models(getModels());
            return models.findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModelList CModelDataReader::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode)
        {
            if (aircraftDesignator.isEmpty()) { return CAircraftModelList(); }
            const CAircraftModelList models(getModels());
            return models.findByAircraftDesignatorAndLiveryCombinedCode(aircraftDesignator, combinedCode);
        }

        int CModelDataReader::getLiveriesCount() const
        {
            return this->getLiveries().size();
        }

        int CModelDataReader::getDistributorsCount() const
        {
            return this->getDistributors().size();
        }

        CDistributor CModelDataReader::smartDistributorSelector(const CDistributor &distributorPattern) const
        {
            const CDistributorList distributors(getDistributors()); // thread safe copy
            return distributors.smartDistributorSelector(distributorPattern);
        }

        CDistributor CModelDataReader::smartDistributorSelector(const CDistributor &distributorPattern, const CAircraftModel &model) const
        {
            const CDistributorList distributors(getDistributors()); // thread safe copy
            return distributors.smartDistributorSelector(distributorPattern, model);
        }

        int CModelDataReader::getModelsCount() const
        {
            return this->getModels().size();
        }

        QSet<int> CModelDataReader::getModelDbKeys() const
        {
            return this->getModels().toDbKeySet();
        }

        QStringList CModelDataReader::getModelStringList() const
        {
            return this->getModels().getModelStringList(false);
        }

        bool CModelDataReader::areAllDataRead() const
        {
            return
                getLiveriesCount() > 0 &&
                getModelsCount() > 0 &&
                getDistributorsCount() > 0;
        }

        void CModelDataReader::ps_read(CEntityFlags::Entity entity, const QDateTime &newerThan)
        {
            this->threadAssertCheck();
            if (this->isAbandoned()) { return; }

            CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
            if (entity.testFlag(CEntityFlags::LiveryEntity))
            {
                CUrl url(getLiveryUrl());
                if (!url.isEmpty())
                {
                    if (!newerThan.isNull())
                    {
                        const QString tss(newerThan.toString(Qt::ISODate));
                        url.appendQuery(QString(parameterLatestTimestamp() + "=" + tss));
                    }
                    sApp->getFromNetwork(url, { this, &CModelDataReader::ps_parseLiveryData});
                    triggeredRead |= CEntityFlags::LiveryEntity;
                }
                else
                {
                    CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::LiveryEntity);
                }
            }

            if (entity.testFlag(CEntityFlags::DistributorEntity))
            {
                CUrl url(getDistributorUrl());
                if (!url.isEmpty())
                {
                    if (!newerThan.isNull())
                    {
                        const QString tss(newerThan.toString(Qt::ISODate));
                        url.appendQuery(QString(parameterLatestTimestamp() + "=" + tss));
                    }
                    sApp->getFromNetwork(url, { this, &CModelDataReader::ps_parseDistributorData});
                    triggeredRead |= CEntityFlags::DistributorEntity;
                }
                else
                {
                    CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::DistributorEntity);
                }
            }

            if (entity.testFlag(CEntityFlags::ModelEntity))
            {
                CUrl url(getModelUrl());
                if (!url.isEmpty())
                {
                    if (!newerThan.isNull())
                    {
                        const QString tss(newerThan.toString(Qt::ISODate));
                        url.appendQuery(QString(parameterLatestTimestamp() + "=" + tss));
                    }
                    sApp->getFromNetwork(url, { this, &CModelDataReader::ps_parseModelData});
                    triggeredRead |= CEntityFlags::ModelEntity;
                }
                else
                {
                    CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::ModelEntity);
                }
            }

            if (triggeredRead != CEntityFlags::NoEntity)
            {
                emit dataRead(triggeredRead, CEntityFlags::StartRead, 0);
            }
        }

        void CModelDataReader::ps_liveryCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::LiveryEntity);
        }

        void CModelDataReader::ps_modelCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::ModelEntity);
        }

        void CModelDataReader::ps_distributorCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::DistributorEntity);
        }

        void CModelDataReader::ps_baseUrlCacheChanged()
        {
            // void
        }

        void CModelDataReader::updateReaderUrl(const CUrl &url)
        {
            const CUrl current = this->m_readerUrlCache.get();
            if (current == url) { return; }
            const CStatusMessage m = this->m_readerUrlCache.set(url);
            if (m.isFailure())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CModelDataReader::ps_parseLiveryData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (this->isAbandoned()) { return; }
            QString urlString(nwReply->url().toString());
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit dataRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set of distributor
            CLiveryList liveries;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                liveries = this->getLiveries();
                liveries.replaceOrAddObjectsByKey(CLiveryList::fromDatabaseJson(res));
            }
            else
            {
                liveries  = CLiveryList::fromDatabaseJson(res);
            }

            const int n = liveries.size();
            qint64 latestTimestamp = liveries.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in livery list, setting to last modified value");
                latestTimestamp = lastModifiedMsSinceEpoch(nwReply.data());
            }
            this->m_liveryCache.set(liveries, latestTimestamp);
            this->updateReaderUrl(getBaseUrl());

            // never emit when lock is held -> deadlock
            emit dataRead(CEntityFlags::LiveryEntity, res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
            CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::LiveryEntity) << urlString;
        }

        void CModelDataReader::ps_parseDistributorData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (this->isAbandoned()) { return; }
            QString urlString(nwReply->url().toString());
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set of distributors
            CDistributorList distributors;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                distributors = this->getDistributors();
                distributors.replaceOrAddObjectsByKey(CDistributorList::fromDatabaseJson(res));
            }
            else
            {
                distributors = CDistributorList::fromDatabaseJson(res);
            }

            const int n = distributors.size();
            qint64 latestTimestamp = distributors.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in distributor list, setting to last modified value");
                latestTimestamp = lastModifiedMsSinceEpoch(nwReply.data());
            }
            this->m_distributorCache.set(distributors, latestTimestamp);
            this->updateReaderUrl(getBaseUrl());

            emit dataRead(CEntityFlags::DistributorEntity, res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
            CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::DistributorEntity) << urlString;
        }

        void CModelDataReader::ps_parseModelData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (this->isAbandoned()) { return; }
            QString urlString(nwReply->url().toString());
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit dataRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set of models
            CAircraftModelList models;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                models = this->getModels();
                models.replaceOrAddObjectsByKey(CAircraftModelList::fromDatabaseJson(res));
            }
            else
            {
                models = CAircraftModelList::fromDatabaseJson(res);
            }

            // syncronized update
            const int n = models.size();
            qint64 latestTimestamp = models.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in model list, setting to last modified value");
                latestTimestamp = lastModifiedMsSinceEpoch(nwReply.data());
            }
            this->m_modelCache.set(models, latestTimestamp);
            this->updateReaderUrl(getBaseUrl());

            emit dataRead(CEntityFlags::ModelEntity, res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
            CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::ModelEntity) << urlString;
        }

        bool CModelDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead)
        {
            QDir directory(dir);
            if (!directory.exists()) { return false; }
            BlackMisc::Network::CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;

            if (whatToRead.testFlag(CEntityFlags::LiveryEntity))
            {
                QString liveriesJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "liveries.json")));
                if (!liveriesJson.isEmpty())
                {
                    CLiveryList liveries;
                    liveries.convertFromJson(liveriesJson);
                    const int c = liveries.size();
                    this->m_liveryCache.set(liveries);

                    emit dataRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, c);
                    reallyRead |= CEntityFlags::LiveryEntity;
                }
            }

            if (whatToRead.testFlag(CEntityFlags::ModelEntity))
            {
                QString modelsJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "models.json")));
                if (!modelsJson.isEmpty())
                {
                    CAircraftModelList models;
                    models.convertFromJson(Json::jsonObjectFromString(modelsJson));
                    const int c = models.size();
                    this->m_modelCache.set(models);

                    emit dataRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFinished, c);
                    reallyRead |= CEntityFlags::ModelEntity;
                }
            }

            if (whatToRead.testFlag(CEntityFlags::DistributorEntity))
            {
                QString distributorsJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "distributors.json")));
                if (!distributorsJson.isEmpty())
                {
                    CDistributorList distributors;
                    distributors.convertFromJson(Json::jsonObjectFromString(distributorsJson));
                    const int c = distributors.size();
                    this->m_distributorCache.set(distributors);

                    emit dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, c);
                    reallyRead |= CEntityFlags::DistributorEntity;
                }
            }

            return (reallyRead & CEntityFlags::DistributorLiveryModel) == whatToRead;
        }

        bool CModelDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead)
        {
            if (dir.isEmpty() || whatToRead == CEntityFlags::NoEntity) { return false; }
            QTimer::singleShot(0, this, [this, dir, whatToRead]()
            {
                bool s = this->readFromJsonFiles(dir, whatToRead);
                Q_UNUSED(s);
            });
            return true;
        }

        bool CModelDataReader::writeToJsonFiles(const QString &dir) const
        {
            QDir directory(dir);
            if (!directory.exists()) { return false; }
            if (this->getLiveriesCount() > 0)
            {
                QString json(QJsonDocument(this->getLiveries().toJson()).toJson());
                bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "liveries.json"));
                if (!s) { return false; }
            }

            if (this->getModelsCount() > 0)
            {
                QString json(QJsonDocument(this->getModels().toJson()).toJson());
                bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "models.json"));
                if (!s) { return false; }
            }

            if (this->getDistributorsCount() > 0)
            {
                QString json(QJsonDocument(this->getDistributors().toJson()).toJson());
                bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "distributors.json"));
                if (!s) { return false; }
            }
            return true;
        }

        void CModelDataReader::synchronizeCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { this->m_liveryCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { this->m_modelCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { this->m_distributorCache.synchronize(); }
        }

        void CModelDataReader::admitCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { this->m_liveryCache.admit(); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { this->m_modelCache.admit(); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { this->m_distributorCache.admit(); }
        }

        void CModelDataReader::invalidateCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { CDataCache::instance()->clearAllValues(this->m_liveryCache.getKey()); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { CDataCache::instance()->clearAllValues(this->m_modelCache.getKey()); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { CDataCache::instance()->clearAllValues(this->m_distributorCache.getKey()); }
        }

        QDateTime CModelDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::LiveryEntity:      return this->m_liveryCache.getAvailableTimestamp();
            case CEntityFlags::ModelEntity:       return this->m_modelCache.getAvailableTimestamp();
            case CEntityFlags::DistributorEntity: return this->m_distributorCache.getAvailableTimestamp();
            default: return QDateTime();
            }
        }

        int CModelDataReader::getCacheCount(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::LiveryEntity:      return this->m_liveryCache.get().size();
            case CEntityFlags::ModelEntity:       return this->m_modelCache.get().size();
            case CEntityFlags::DistributorEntity: return this->m_distributorCache.get().size();
            default: return 0;
            }
        }

        bool CModelDataReader::hasChangedUrl(CEntityFlags::Entity entity) const
        {
            Q_UNUSED(entity);
            return CDatabaseReader::isChangedUrl(this->m_readerUrlCache.get(), getBaseUrl());
        }

        const CUrl &CModelDataReader::getBaseUrl()
        {
            static const CUrl baseUrl(sApp->getGlobalSetup().getDbModelReaderUrl());
            return baseUrl;
        }

        CUrl CModelDataReader::getLiveryUrl(bool shared) const
        {
            return shared ?
                   getBaseUrl().withAppendedPath("service/jsonlivery.php") :
                   getBaseUrl().withAppendedPath("service/jsonlivery.php");
        }

        CUrl CModelDataReader::getDistributorUrl(bool shared) const
        {
            return shared ?
                   getBaseUrl().withAppendedPath("service/jsondistributor.php") :
                   getBaseUrl().withAppendedPath("service/jsondistributor.php");
        }

        CUrl CModelDataReader::getModelUrl(bool shared) const
        {
            return shared ?
                   getBaseUrl().withAppendedPath("service/jsonaircraftmodel.php") :
                   getBaseUrl().withAppendedPath("service/jsonaircraftmodel.php");
        }
    } // ns
} // ns
