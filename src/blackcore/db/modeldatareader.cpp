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
#include "blackcore/db/databaseutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"

#include <QDir>
#include <QFlags>
#include <QFileInfo>
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
using namespace BlackMisc::Db;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackCore::Data;
using namespace BlackCore::Db;

namespace BlackCore
{
    namespace Db
    {
        CModelDataReader::CModelDataReader(QObject *owner, const CDatabaseReaderConfigList &config) :
            CDatabaseReader(owner, config, "CModelDataReader")
        {
            // init to avoid threading issues
            getBaseUrl(CDbFlags::DbReading);
        }

        CLiveryList CModelDataReader::getLiveries() const
        {
            return m_liveryCache.get();
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

        CDistributor CModelDataReader::getDistributorForDbKey(const QString &dbKey) const
        {
            if (dbKey.isEmpty()) { return CDistributor(); }
            const CDistributorList distributors(getDistributors());
            return distributors.findByKeyOrAlias(dbKey);
        }

        CAircraftModelList CModelDataReader::getModels() const
        {
            return m_modelCache.get();
        }

        CAircraftModel CModelDataReader::getModelForModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            const CAircraftModelList models(this->getModels());
            return models.findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModel CModelDataReader::getModelForDbKey(int dbKey) const
        {
            if (dbKey < 0) { return CAircraftModel(); }
            const CAircraftModelList models(this->getModels());
            return models.findByKey(dbKey);
        }

        QSet<QString> CModelDataReader::getAircraftDesignatorsForAirline(const CAirlineIcaoCode &code) const
        {
            if (!code.hasValidDesignator()) { return QSet<QString>(); }
            const CAircraftModelList models(this->getModels());
            return models.getAircraftDesignatorsForAirline(code);
        }

        CAircraftIcaoCodeList CModelDataReader::getAicraftIcaoCodesForAirline(const CAirlineIcaoCode &code) const
        {
            if (!code.hasValidDesignator()) { return CAircraftIcaoCodeList(); }
            const CAircraftModelList models(this->getModels());
            return models.getAicraftIcaoCodesForAirline(code);
        }

        CAircraftModelList CModelDataReader::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode)
        {
            if (aircraftDesignator.isEmpty()) { return CAircraftModelList(); }
            const CAircraftModelList models(this->getModels());
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

        QStringList CModelDataReader::getModelStringList(bool sort) const
        {
            return this->getModels().getModelStringList(sort);
        }

        bool CModelDataReader::areAllDataRead() const
        {
            return
                this->getLiveriesCount() > 0 &&
                this->getModelsCount() > 0 &&
                this->getDistributorsCount() > 0;
        }

        void CModelDataReader::ps_read(CEntityFlags::Entity entities, CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan)
        {
            this->threadAssertCheck();
            if (!this->doWorkCheck()) { return; }
            entities &= CEntityFlags::DistributorLiveryModel;
            if (!this->isInternetAccessible())
            {
                emit this->dataRead(entities, CEntityFlags::ReadSkipped, 0);
                return;
            }

            CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
            if (entities.testFlag(CEntityFlags::LiveryEntity))
            {
                CUrl url(getLiveryUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CModelDataReader::ps_parseLiveryData});
                    triggeredRead |= CEntityFlags::LiveryEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::LiveryEntity);
                }
            }

            if (entities.testFlag(CEntityFlags::DistributorEntity))
            {
                CUrl url(getDistributorUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CModelDataReader::ps_parseDistributorData});
                    triggeredRead |= CEntityFlags::DistributorEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::DistributorEntity);
                }
            }

            if (entities.testFlag(CEntityFlags::ModelEntity))
            {
                CUrl url(getModelUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CModelDataReader::ps_parseModelData});
                    triggeredRead |= CEntityFlags::ModelEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::ModelEntity);
                }
            }

            if (triggeredRead != CEntityFlags::NoEntity)
            {
                emit dataRead(triggeredRead, CEntityFlags::StartRead, 0);
            }
        }

        void CModelDataReader::liveryCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::LiveryEntity);
        }

        void CModelDataReader::modelCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::ModelEntity);
        }

        void CModelDataReader::distributorCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::DistributorEntity);
        }

        void CModelDataReader::baseUrlCacheChanged()
        {
            // void
        }

        void CModelDataReader::updateReaderUrl(const CUrl &url)
        {
            const CUrl current = m_readerUrlCache.get();
            if (current == url) { return; }
            const CStatusMessage m = m_readerUrlCache.set(url);
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
            if (!this->doWorkCheck()) { return; }
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
                const CLiveryList incrementalLiveries(CLiveryList::fromDatabaseJson(res));
                if (incrementalLiveries.isEmpty()) { return; } // currenty ignored
                liveries = this->getLiveries();
                liveries.replaceOrAddObjectsByKey(incrementalLiveries);
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
            m_liveryCache.set(liveries, latestTimestamp);
            this->updateReaderUrl(getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::LiveryEntity, n, res);
        }

        void CModelDataReader::ps_parseDistributorData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (!this->doWorkCheck()) { return; }
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
                const CDistributorList incrementalDistributors(CDistributorList::fromDatabaseJson(res));
                if (incrementalDistributors.isEmpty()) { return; } // currently ignored
                distributors = this->getDistributors();
                distributors.replaceOrAddObjectsByKey(incrementalDistributors);
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
            m_distributorCache.set(distributors, latestTimestamp);
            this->updateReaderUrl(getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::DistributorEntity, n, res);
        }

        void CModelDataReader::ps_parseModelData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (!this->doWorkCheck()) { return; }
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit this->dataRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set of models
            CAircraftModelList models;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                const CAircraftModelList incrementalModels(CAircraftModelList::fromDatabaseJson(res));
                if (incrementalModels.isEmpty()) { return; } // currently ignored
                models = this->getModels();
                models.replaceOrAddObjectsByKey(incrementalModels);
            }
            else
            {
                models = CAircraftModelList::fromDatabaseJson(res);
            }

            // synchronized update
            const int n = models.size();
            qint64 latestTimestamp = models.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in model list, setting to last modified value");
                latestTimestamp = lastModifiedMsSinceEpoch(nwReply.data());
            }
            m_modelCache.set(models, latestTimestamp);
            this->updateReaderUrl(getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::ModelEntity, n, res);
        }

        CStatusMessageList CModelDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
        {
            const QDir directory(dir);
            if (!directory.exists())
            {
                return CStatusMessage(this).error("Missing directory '%1'") << dir;
            }

            whatToRead &= CEntityFlags::DistributorLiveryModel; // supported
            BlackMisc::Network::CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;

            CStatusMessageList msgs;
            if (whatToRead.testFlag(CEntityFlags::LiveryEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "liveries.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::LiveryEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject liveriesJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (liveriesJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CLiveryList liveries = CLiveryList::fromMultipleJsonFormats(liveriesJson);
                            const int c = liveries.size();
                            msgs.push_back(m_liveryCache.set(liveries, fi.created().toUTC().toMSecsSinceEpoch()));
                            emit this->dataRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, c);
                            reallyRead |= CEntityFlags::LiveryEntity;
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading liveries from '%1'").arg(fileName)));
                        }
                    }
                }
            }

            if (whatToRead.testFlag(CEntityFlags::ModelEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "models.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::ModelEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject modelsJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (modelsJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CAircraftModelList models = CAircraftModelList::fromMultipleJsonFormats(modelsJson);
                            const int c = models.size();
                            msgs.push_back(m_modelCache.set(models, fi.created().toUTC().toMSecsSinceEpoch()));
                            emit this->dataRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFinished, c);
                            reallyRead |= CEntityFlags::ModelEntity;
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::ModelEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading models from '%1'").arg(fileName)));
                        }
                    }
                }
            }

            if (whatToRead.testFlag(CEntityFlags::DistributorEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "distributors.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::DistributorEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject distributorsJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (distributorsJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CDistributorList distributors = CDistributorList::fromMultipleJsonFormats(distributorsJson);
                            const int c = distributors.size();
                            msgs.push_back(m_distributorCache.set(distributors, fi.created().toUTC().toMSecsSinceEpoch()));
                            emit this->dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, c);
                            reallyRead |= CEntityFlags::DistributorEntity;
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading distributors from '%1'").arg(fileName)));
                        }
                    }
                }
            }

            return msgs;
        }

        bool CModelDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
        {
            if (dir.isEmpty() || whatToRead == CEntityFlags::NoEntity) { return false; }
            QTimer::singleShot(0, this, [ = ]()
            {
                const CStatusMessageList msgs = this->readFromJsonFiles(dir, whatToRead, overrideNewerOnly);
                if (msgs.isFailure())
                {
                    CLogMessage::preformatted(msgs);
                }
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

        CEntityFlags::Entity CModelDataReader::getSupportedEntities() const
        {
            return CEntityFlags::DistributorLiveryModel;
        }

        void CModelDataReader::synchronizeCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { m_liveryCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { m_modelCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { m_distributorCache.synchronize(); }
        }

        void CModelDataReader::admitCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { m_liveryCache.admit(); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { m_modelCache.admit(); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { m_distributorCache.admit(); }
        }

        void CModelDataReader::invalidateCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::LiveryEntity)) { CDataCache::instance()->clearAllValues(m_liveryCache.getKey()); }
            if (entities.testFlag(CEntityFlags::ModelEntity))  { CDataCache::instance()->clearAllValues(m_modelCache.getKey()); }
            if (entities.testFlag(CEntityFlags::DistributorEntity)) { CDataCache::instance()->clearAllValues(m_distributorCache.getKey()); }
        }

        QDateTime CModelDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::LiveryEntity:      return m_liveryCache.getAvailableTimestamp();
            case CEntityFlags::ModelEntity:       return m_modelCache.getAvailableTimestamp();
            case CEntityFlags::DistributorEntity: return m_distributorCache.getAvailableTimestamp();
            default: return QDateTime();
            }
        }

        int CModelDataReader::getCacheCount(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::LiveryEntity:      return m_liveryCache.get().size();
            case CEntityFlags::ModelEntity:       return m_modelCache.get().size();
            case CEntityFlags::DistributorEntity: return m_distributorCache.get().size();
            default: return 0;
            }
        }

        CEntityFlags::Entity CModelDataReader::getEntitiesWithCacheCount() const
        {
            CEntityFlags::Entity entities = CEntityFlags::NoEntity;
            if (this->getCacheCount(CEntityFlags::LiveryEntity) > 0) entities |= CEntityFlags::LiveryEntity;
            if (this->getCacheCount(CEntityFlags::ModelEntity) > 0) entities |= CEntityFlags::ModelEntity;
            if (this->getCacheCount(CEntityFlags::DistributorEntity) > 0) entities |= CEntityFlags::DistributorEntity;
            return entities;
        }

        CEntityFlags::Entity CModelDataReader::getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const
        {
            CEntityFlags::Entity entities = CEntityFlags::NoEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::LiveryEntity, threshold)) entities |= CEntityFlags::LiveryEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::ModelEntity, threshold)) entities |= CEntityFlags::ModelEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::DistributorEntity, threshold)) entities |= CEntityFlags::DistributorEntity;
            return entities;
        }

        bool CModelDataReader::hasChangedUrl(CEntityFlags::Entity entity, CUrl &oldUrlInfo, CUrl &newUrlInfo) const
        {
            Q_UNUSED(entity);
            oldUrlInfo = m_readerUrlCache.get();
            newUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
            return CDatabaseReader::isChangedUrl(oldUrlInfo, newUrlInfo);
        }

        CUrl CModelDataReader::getDbServiceBaseUrl() const
        {
            return sApp->getGlobalSetup().getDbModelReaderUrl();
        }

        CUrl CModelDataReader::getLiveryUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::LiveryEntity, mode));
        }

        CUrl CModelDataReader::getDistributorUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::DistributorEntity, mode));
        }

        CUrl CModelDataReader::getModelUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::ModelEntity, mode));
        }
    } // ns
} // ns
