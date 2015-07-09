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
#include "modeldatareader.h"

#include <QRegularExpression>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

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

    int CModelDataReader::getModelsCount() const
    {
        QReadLocker l(&m_lockModels);
        return m_models.size();
    }

    bool CModelDataReader::allRead() const
    {
        return
            getLiveriesCount() > 0 &&
            getModelsCount() > 0 &&
            getDistributorsCount();
    }

    void CModelDataReader::ps_read()
    {
        this->threadAssertCheck();
        Q_ASSERT(this->m_networkManagerLivery);
        Q_ASSERT(this->m_networkManagerDistributor);
        Q_ASSERT(this->m_networkManagerModel);
        Q_ASSERT(!m_urlLiveries.isEmpty());
        Q_ASSERT(!m_urlDistributors.isEmpty());
        QNetworkRequest requestLivery(m_urlLiveries);
        QNetworkRequest requestDistributor(m_urlDistributors);
        QNetworkRequest requestModel(m_urlModels);
        this->m_networkManagerLivery->get(requestLivery);
        this->m_networkManagerDistributor->get(requestDistributor);
        this->m_networkManagerModel->get(requestModel);
    }

    QString CModelDataReader::getLiveryUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return buildUrl(protocol, server, baseUrl, "service/alllivery.php?rows=20000&sord=asc");
    }

    QString CModelDataReader::getDistributorUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return buildUrl(protocol, server, baseUrl, "service/alldistributor.php?rows=20000&sord=asc");
    }

    QString CModelDataReader::getModelUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return buildUrl(protocol, server, baseUrl, "service/allaircraftmodel.php?rows=20000&sord=asc");
    }

    void CModelDataReader::ps_parseLiveryData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoJsonArray(nwReply.data());
        if (array.isEmpty()) { return; }
        CLiveryList liveries = CLiveryList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockLivery);
            this->m_liveries = liveries;
            n = liveries.size();
        }
        emit readLiveries(n);
        if (allRead()) { emit readAll(); }
    }

    void CModelDataReader::ps_parseDistributorData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoJsonArray(nwReply.data());
        if (array.isEmpty()) { return; }
        CDistributorList distributors = CDistributorList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockDistributor);
            this->m_distributors = distributors;
            n = distributors.size();
        }
        emit readDistributors(n);
        if (allRead()) { emit readAll(); }
    }

    void CModelDataReader::ps_parseModelData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoJsonArray(nwReply.data());
        if (array.isEmpty()) { return; }
        CAircraftModelList models = CAircraftModelList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockModels);
            this->m_models = models;
            n = models.size();
        }
        emit readModels(n);
        if (allRead()) { emit readAll(); }
    }

} // namespace
