/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/networkutils.h"
#include "modeldatareader.h"

#include <QRegularExpression>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CModelDataReader::CModelDataReader(QObject *owner, const QString &protocol, const QString &server, const QString &baseUrl) :
        CDatabaseReader(owner, "CModelDataReader"),
        m_urlLiveries(getLiveryUrl(protocol, server, baseUrl)),
        m_urlDistributors(getDistributorUrl(protocol, server, baseUrl)),
        m_urlModels(getModelUrl(protocol, server, baseUrl))
    {
        this->m_networkManagerLivery = new QNetworkAccessManager(this);
        this->m_networkManagerDistributor = new QNetworkAccessManager(this);
        this->m_networkManagerModel = new QNetworkAccessManager(this);

        this->connect(this->m_networkManagerLivery, &QNetworkAccessManager::finished, this, &CModelDataReader::ps_parseLiveryData);
        this->connect(this->m_networkManagerDistributor, &QNetworkAccessManager::finished, this, &CModelDataReader::ps_parseDistributorData);
        this->connect(this->m_networkManagerModel, &QNetworkAccessManager::finished, this, &CModelDataReader::ps_parseModelData);
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

    CLivery CModelDataReader::getLiveryForDbKey(int id) const
    {
        if (id < 0) { return CLivery(); }
        CLiveryList liveries(getLiveries());
        return liveries.findByKey(id);
    }

    CLivery CModelDataReader::smartLiverySelector(const CLivery &livery) const
    {
        CLiveryList liveries(getLiveries()); // thread safe copy

        // first try on id, that would be perfect
        if (livery.hasValidDbKey())
        {
            int k = livery.getDbKey();
            CLivery l(liveries.findByKey(k));
            if (l.hasCompleteData()) { return l; }
        }

        // by combined code
        if (livery.hasCombinedCode())
        {
            QString cc(livery.getCombinedCode());
            CLivery l(liveries.findByCombinedCode(cc));
            if (l.hasCompleteData()) { return l; }
        }

        if (livery.hasValidAirlineDesignator())
        {
            QString icao(livery.getAirlineIcaoCodeDesignator());
            CLivery l(liveries.findByAirlineIcaoDesignatorStdLivery(icao));
            if (l.hasCompleteData()) { return l; }
        }

        return CLivery();
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

    CDistributor CModelDataReader::smartDistributorSelector(const CDistributor &distributor) const
    {
        CDistributorList distributors(getDistributors()); // thread safe copy
        if (distributor.hasValidDbKey())
        {
            QString k(distributor.getDbKey());
            CDistributor d(distributors.findByKey(k));
            if (d.hasCompleteData()) { return d; }

            // more lenient search
            return distributors.findByIdOrAlias(k);
        }
        return CDistributor();
    }

    int CModelDataReader::getModelsCount() const
    {
        QReadLocker l(&m_lockModels);
        return m_models.size();
    }

    bool CModelDataReader::areAllDataRead() const
    {
        return
            getLiveriesCount() > 0 &&
            getModelsCount() > 0 &&
            getDistributorsCount() > 0;
    }

    bool CModelDataReader::canConnect(QString &message) const
    {
        if (m_urlDistributors.isEmpty() || m_urlLiveries.isEmpty() || m_urlModels.isEmpty()) { return false; }
        bool cm = CNetworkUtils::canConnect(m_urlModels, message);

        // currently only testing one URL, might be changed in the future
        return cm;
    }

    void CModelDataReader::ps_read(CDbFlags::Entity entity)
    {
        this->threadAssertCheck();
        Q_ASSERT(this->m_networkManagerLivery);
        Q_ASSERT(this->m_networkManagerDistributor);
        Q_ASSERT(this->m_networkManagerModel);
        Q_ASSERT(!m_urlLiveries.isEmpty());
        Q_ASSERT(!m_urlDistributors.isEmpty());

        CDbFlags::Entity triggeredRead = CDbFlags::NoEntity;
        if (entity.testFlag(CDbFlags::LiveryEntity))
        {
            QNetworkRequest requestLivery(m_urlLiveries);
            this->m_networkManagerLivery->get(requestLivery);
            triggeredRead |= CDbFlags::LiveryEntity;
        }

        if (entity.testFlag(CDbFlags::DistributorEntity))
        {
            QNetworkRequest requestDistributor(m_urlDistributors);
            this->m_networkManagerDistributor->get(requestDistributor);
            triggeredRead |= CDbFlags::DistributorEntity;
        }

        if (entity.testFlag(CDbFlags::ModelEntity))
        {
            QNetworkRequest requestModel(m_urlModels);
            this->m_networkManagerModel->get(requestModel);
            triggeredRead |= CDbFlags::ModelEntity;
        }

        emit readData(triggeredRead, CDbFlags::StartRead, 0);
    }

    void CModelDataReader::ps_parseLiveryData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit readData(CDbFlags::LiveryEntity, CDbFlags::ReadFailed, 0);
            return;
        }
        CLiveryList liveries = CLiveryList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n = 0;
        {
            QWriteLocker wl(&this->m_lockLivery);
            this->m_liveries = liveries;
            n = liveries.size();
        }
        emit readData(CDbFlags::LiveryEntity, CDbFlags::ReadFinished, n);
    }

    void CModelDataReader::ps_parseDistributorData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit readData(CDbFlags::DistributorEntity, CDbFlags::ReadFailed, 0);
            return;
        }
        CDistributorList distributors = CDistributorList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n = 0;
        {
            QWriteLocker wl(&this->m_lockDistributor);
            this->m_distributors = distributors;
            n = distributors.size();
        }
        emit readData(CDbFlags::DistributorEntity, CDbFlags::ReadFinished, n);
    }

    void CModelDataReader::ps_parseModelData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit readData(CDbFlags::ModelEntity, CDbFlags::ReadFailed, 0);
            return;
        }
        CAircraftModelList models = CAircraftModelList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n = 0;
        {
            QWriteLocker wl(&this->m_lockModels);
            this->m_models = models;
            n = models.size();
        }
        emit readData(CDbFlags::ModelEntity, CDbFlags::ReadFinished, n);
    }

    QString CModelDataReader::getLiveryUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsonlivery.php");
    }

    QString CModelDataReader::getDistributorUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsondistributor.php");
    }

    QString CModelDataReader::getModelUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsonaircraftmodel.php");
    }

} // namespace
