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
    CIcaoDataReader::CIcaoDataReader(QObject *owner, const QString &aircraftIcaoUrl, const QString &airlineIcaoUrl) :
        CThreadedReader(owner, "CIcaoDataReader"),
        m_urlAircraftIcao(aircraftIcaoUrl), m_urlAirlineIcao(airlineIcaoUrl)
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

    void CIcaoDataReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_read");
        Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
        Q_UNUSED(s);
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

    QJsonArray CIcaoDataReader::splitReplyIntoArray(QNetworkReply *nwReply) const
    {
        this->threadAssertCheck();
        QJsonArray array;
        if (this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated ICAO data parsing process"); // for users
            return array; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap
            if (dataFileData.isEmpty()) { return array; }

            QJsonDocument jsonResponse = QJsonDocument::fromJson(dataFileData.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            QJsonArray jsonArray = jsonObject["rows"].toArray();
            return jsonArray;
        }
        CLogMessage(this).warning("Reading data failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
        nwReply->abort();
        return array;
    }

    bool BlackCore::CIcaoDataReader::checkIfFinished() const
    {
        if (!this->isFinished()) { return false; }
        CLogMessage(this).debug() << Q_FUNC_INFO;
        CLogMessage(this).info("Terminated ICAO data parsing process"); // for users
        return true;
    }

    void CIcaoDataReader::ps_parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->splitReplyIntoArray(nwReply.data());
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
    }

    void CIcaoDataReader::ps_parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->splitReplyIntoArray(nwReply.data());
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
    }

} // namespace
