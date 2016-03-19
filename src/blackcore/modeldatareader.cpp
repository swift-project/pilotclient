/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/fileutils.h"
#include "modeldatareader.h"

#include <QTimer>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore
{
    CModelDataReader::CModelDataReader(QObject *owner) :
        CDatabaseReader(owner, "CModelDataReader")
    {
        // void
    }

    CLiveryList CModelDataReader::getLiveries() const
    {
        QReadLocker l(&m_lockLivery);
        return m_liveries;
    }

    CLivery CModelDataReader::getLiveryForCombinedCode(const QString &combinedCode) const
    {
        if (!CLivery::isValidCombinedCode(combinedCode)) { return CLivery(); }
        CLiveryList liveries(getLiveries());
        return liveries.findByCombinedCode(combinedCode);
    }

    CLivery CModelDataReader::getStdLiveryForAirlineCode(const CAirlineIcaoCode &icao) const
    {
        if (!icao.hasValidDesignator()) { return CLivery(); }
        CLiveryList liveries(getLiveries());
        return liveries.findStdLiveryByAirlineIcaoDesignator(icao);
    }

    CLivery CModelDataReader::getLiveryForDbKey(int id) const
    {
        if (id < 0) { return CLivery(); }
        CLiveryList liveries(getLiveries());
        return liveries.findByKey(id);
    }

    CLivery CModelDataReader::smartLiverySelector(const CLivery &liveryPattern) const
    {
        CLiveryList liveries(getLiveries()); // thread safe copy
        return liveries.smartLiverySelector(liveryPattern);
    }

    CDistributorList CModelDataReader::getDistributors() const
    {
        QReadLocker l(&m_lockDistributor);
        return m_distributors;
    }

    CAircraftModelList CModelDataReader::getModels() const
    {
        QReadLocker l(&m_lockModels);
        return m_models;
    }

    CAircraftModel CModelDataReader::getModelForModelString(const QString &modelString) const
    {
        if (modelString.isEmpty()) { return CAircraftModel(); }
        CAircraftModelList models(getModels());
        return models.findFirstByModelString(modelString);
    }

    CAircraftModelList CModelDataReader::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode)
    {
        if (aircraftDesignator.isEmpty()) { return CAircraftModelList(); }
        CAircraftModelList models(getModels());
        return models.findByAircraftDesignatorAndLiveryCombinedCode(aircraftDesignator, combinedCode);
    }

    int CModelDataReader::getLiveriesCount() const
    {
        QReadLocker l(&m_lockLivery);
        return m_liveries.size();
    }

    int CModelDataReader::getDistributorsCount() const
    {
        QReadLocker l(&m_lockDistributor);
        return m_distributors.size();
    }

    CDistributor CModelDataReader::smartDistributorSelector(const CDistributor &distributorPattern) const
    {
        CDistributorList distributors(getDistributors()); // thread safe copy
        return distributors.smartDistributorSelector(distributorPattern);
    }

    int CModelDataReader::getModelsCount() const
    {
        QReadLocker l(&m_lockModels);
        return m_models.size();
    }

    QList<int> CModelDataReader::getModelDbKeys() const
    {
        QReadLocker l(&m_lockModels);
        return m_models.toDbKeyList();
    }

    QStringList CModelDataReader::getModelStrings() const
    {
        QReadLocker l(&m_lockModels);
        return m_models.getModelStrings(false);
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

    void CModelDataReader::ps_parseLiveryData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
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

        // this part needs to be synchronized
        int n = liveries.size();
        {
            QWriteLocker wl(&this->m_lockLivery);
            this->m_liveries = liveries;
        }

        // never emit when lock is held -> deadlock
        emit dataRead(CEntityFlags::LiveryEntity,
                      res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
        CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::LiveryEntity) << urlString;
    }

    void CModelDataReader::ps_parseDistributorData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QString urlString(nwReply->url().toString());
        CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFailed, 0);
            return;
        }

        // get all or incremental set of distributor
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

        // this part needs to be synchronized
        int n = distributors.size();
        {
            QWriteLocker wl(&this->m_lockDistributor);
            this->m_distributors = distributors;
        }
        emit dataRead(CEntityFlags::DistributorEntity,
                      res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
        CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::DistributorEntity) << urlString;
    }

    void CModelDataReader::ps_parseModelData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
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
        int n = models.size();
        {
            QWriteLocker wl(&this->m_lockModels);
            this->m_models = models;
        }

        emit dataRead(CEntityFlags::ModelEntity,
                      res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
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
                int c = liveries.size();
                {
                    QWriteLocker l(&m_lockLivery);
                    m_liveries = liveries;
                }
                // never emit when lcok is held -> deadlock
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
                int c = models.size();
                {
                    QWriteLocker l(&m_lockModels);
                    m_models = models;
                }
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
                int c = distributors.size();
                {
                    QWriteLocker l(&m_lockDistributor);
                    m_distributors = distributors;
                }
                reallyRead |= CEntityFlags::DistributorEntity;
                emit dataRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, c);
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

    CUrl CModelDataReader::getBaseUrl() const
    {
        const CUrl baseUrl(sApp->getGlobalSetup().getDbModelReaderUrl());
        return baseUrl;
    }

    CUrl CModelDataReader::getLiveryUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsonlivery.php");
    }

    CUrl CModelDataReader::getDistributorUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsondistributor.php");
    }

    CUrl CModelDataReader::getModelUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsonaircraftmodel.php");
    }

    const QString &CModelDataReader::parameterLatestTimestamp()
    {
        static const QString p("latestTimestamp");
        return p;
    }

    const QString &CModelDataReader::parameterLatestId()
    {
        static const QString p("latestId");
        return p;
    }

} // namespace
