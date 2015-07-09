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
#include "icaodatareader.h"

#include <QRegularExpression>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    CIcaoDataReader::CIcaoDataReader(QObject *owner, const QString &protocol, const QString &server, const QString &baseUrl) :
        CDatabaseReader(owner, "CIcaoDataReader"),
        m_urlAircraftIcao(getAircraftIcaoUrl(protocol, server, baseUrl)), m_urlAirlineIcao(getAirlineIcaoUrl(protocol, server, baseUrl))
    {
        this->m_networkManagerAircraft = new QNetworkAccessManager(this);
        this->m_networkManagerAirlines = new QNetworkAccessManager(this);

        this->connect(this->m_networkManagerAircraft, &QNetworkAccessManager::finished, this, &CIcaoDataReader::ps_parseAircraftIcaoData);
        this->connect(this->m_networkManagerAirlines, &QNetworkAccessManager::finished, this, &CIcaoDataReader::ps_parseAirlineIcaoData);
    }

    CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodes() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftIcaos;
    }

    CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodes() const
    {
        QReadLocker l(&m_lockAirline);
        return m_airlineIcaos;
    }

    int CIcaoDataReader::getAircraftIcaoCodesCount() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftIcaos.size();
    }

    int CIcaoDataReader::getAirlineIcaoCodesCount() const
    {
        QReadLocker l(&m_lockAirline);
        return m_airlineIcaos.size();
    }

    QString CIcaoDataReader::getAircraftIcaoUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return buildUrl(protocol, server, baseUrl, "service/allaircrafticao.php?rows=20000&sord=asc");
    }

    QString CIcaoDataReader::getAirlineIcaoUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return buildUrl(protocol, server, baseUrl, "service/allairlineicao.php?rows=20000&sord=asc");
    }

    void CIcaoDataReader::ps_read()
    {
        this->threadAssertCheck();
        Q_ASSERT(this->m_networkManagerAircraft);
        Q_ASSERT(this->m_networkManagerAirlines);
        Q_ASSERT(!m_urlAircraftIcao.isEmpty());
        Q_ASSERT(!m_urlAirlineIcao.isEmpty());
        QNetworkRequest requestAircraft(m_urlAircraftIcao);
        QNetworkRequest requestAirline(m_urlAirlineIcao);
        this->m_networkManagerAircraft->get(requestAircraft);
        this->m_networkManagerAirlines->get(requestAirline);
    }

    void CIcaoDataReader::ps_parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoJsonArray(nwReply.data());
        if (array.isEmpty()) { return; }
        CAircraftIcaoCodeList codes = CAircraftIcaoCodeList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockAircraft);
            this->m_aircraftIcaos = codes;
            n = codes.size();
        }
        emit readAircraftIcaoCodes(n);
        if (this->getAirlineIcaoCodesCount() > 0) { emit readAll(); }
    }

    void CIcaoDataReader::ps_parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoJsonArray(nwReply.data());
        if (array.isEmpty()) { return; }
        CAirlineIcaoCodeList codes = CAirlineIcaoCodeList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockAirline);
            this->m_airlineIcaos = codes;
            n = codes.size();
        }
        emit readAirlinesIcaoCodes(n);
        if (this->getAircraftIcaoCodesCount() > 0) { emit readAll(); }
    }
} // namespace
