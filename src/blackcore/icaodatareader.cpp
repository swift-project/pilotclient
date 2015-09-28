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
#include "icaodatareader.h"

#include <QRegularExpression>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CIcaoDataReader::CIcaoDataReader(QObject *owner, const QString &protocol, const QString &server, const QString &baseUrl) :
        CDatabaseReader(owner, "CIcaoDataReader"),
        m_urlAircraftIcao(getAircraftIcaoUrl(protocol, server, baseUrl)),
        m_urlAirlineIcao(getAirlineIcaoUrl(protocol, server, baseUrl)),
        m_urlCountry(getCountryUrl(protocol, server, baseUrl))
    {
        this->m_networkManagerAircraft = new QNetworkAccessManager(this);
        this->m_networkManagerAirlines = new QNetworkAccessManager(this);
        this->m_networkManagerCountries = new QNetworkAccessManager(this);

        this->connect(this->m_networkManagerAircraft, &QNetworkAccessManager::finished, this, &CIcaoDataReader::ps_parseAircraftIcaoData);
        this->connect(this->m_networkManagerAirlines, &QNetworkAccessManager::finished, this, &CIcaoDataReader::ps_parseAirlineIcaoData);
        this->connect(this->m_networkManagerCountries, &QNetworkAccessManager::finished, this, &CIcaoDataReader::ps_parseCountryData);
    }

    CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodes() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftIcaos;
    }

    CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDesignator(const QString &designator) const
    {
        return getAircraftIcaoCodes().findFirstByDesignatorAndRank(designator);
    }

    CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDbKey(int key) const
    {
        return getAircraftIcaoCodes().findByKey(key);
    }

    CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodes() const
    {
        QReadLocker l(&m_lockAirline);
        return m_airlineIcaos;
    }

    CAircraftIcaoCode CIcaoDataReader::smartAircraftIcaoSelector(const CAircraftIcaoCode &icao) const
    {
        CAircraftIcaoCodeList codes(getAircraftIcaoCodes()); // thread safe copy
        if (icao.hasValidDbKey())
        {
            int k = icao.getDbKey();
            CAircraftIcaoCode c(codes.findByKey(k));
            if (c.hasCompleteData()) { return c; }
        }

        if (icao.hasKnownDesignator())
        {
            const QString d(icao.getDesignator());
            codes = codes.findByDesignator(d);
            if (codes.size() == 1) { return codes.front(); }
            if (codes.isEmpty()) { return icao; }
            codes.sortByRank();

            // intentionally continue here
        }

        // further reduce by manufacturer
        if (icao.hasManufacturer())
        {
            const QString m(icao.getManufacturer());
            codes = codes.findByManufacturer(m);
            if (codes.size() == 1) { return codes.front(); }
            if (codes.isEmpty()) { return icao; }

            // intentionally continue here
        }

        // lucky punch on description?
        if (icao.hasModelDescription())
        {
            // do not affect codes here, it might return no results
            const QString d(icao.getModelDescription());
            CAircraftIcaoCodeList cm(codes.findByDescription(d));
            if (cm.size() == 1) { return cm.front(); }
            if (cm.size() > 1 && cm.size() < codes.size()) { return codes.front(); }
        }
        return codes.frontOrDefault(); // sorted by rank
    }

    CCountryList CIcaoDataReader::getCountries() const
    {
        QReadLocker l(&m_lockCountry);
        return m_countries;
    }

    CCountry CIcaoDataReader::getCountryForIsoCode(const QString &isoCode) const
    {
        QReadLocker l(&m_lockCountry);
        return m_countries.findByIsoCode(isoCode);
    }

    CCountry CIcaoDataReader::getCountryForName(const QString &name) const
    {
        QReadLocker l(&m_lockCountry);
        return m_countries.findBestMatchByCountryName(name);
    }

    CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForDesignator(const QString &designator) const
    {
        return getAirlineIcaoCodes().findByVDesignator(designator);
    }

    CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForDbKey(int key) const
    {
        return getAirlineIcaoCodes().findByKey(key);
    }

    CAirlineIcaoCode CIcaoDataReader::smartAirlineIcaoSelector(const CAirlineIcaoCode &icao) const
    {
        return icao;
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

    bool CIcaoDataReader::areAllDataRead() const
    {
        return getCountriesCount() > 0 && getAirlineIcaoCodesCount() > 0 && getAircraftIcaoCodesCount() > 0;
    }

    int CIcaoDataReader::getCountriesCount() const
    {
        QReadLocker l(&m_lockCountry);
        return m_countries.size();
    }

    void CIcaoDataReader::ps_read(BlackMisc::Network::CEntityFlags::Entity entities)
    {
        this->threadAssertCheck();
        Q_ASSERT(this->m_networkManagerAircraft);
        Q_ASSERT(this->m_networkManagerAirlines);
        Q_ASSERT(this->m_networkManagerCountries);
        Q_ASSERT(!m_urlAircraftIcao.isEmpty());
        Q_ASSERT(!m_urlAirlineIcao.isEmpty());
        Q_ASSERT(!m_urlCountry.isEmpty());

        CEntityFlags::Entity entitiesTriggered = CEntityFlags::NoEntity;
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            QNetworkRequest requestAircraft(m_urlAircraftIcao);
            this->m_networkManagerAircraft->get(requestAircraft);
            entitiesTriggered |= CEntityFlags::AircraftIcaoEntity;
        }

        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            QNetworkRequest requestAirline(m_urlAirlineIcao);
            this->m_networkManagerAirlines->get(requestAirline);
            entitiesTriggered |= CEntityFlags::AirlineIcaoEntity;
        }

        if (entities.testFlag(CEntityFlags::CountryEntity))
        {
            QNetworkRequest requestCountry(m_urlCountry);
            this->m_networkManagerCountries->get(requestCountry);
            entitiesTriggered |= CEntityFlags::CountryEntity;
        }

        emit dataRead(entitiesTriggered, CEntityFlags::StartRead, 0);
    }

    void CIcaoDataReader::ps_parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CAircraftIcaoCodeList codes = CAircraftIcaoCodeList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockAircraft);
            this->m_aircraftIcaos = codes;
            n = codes.size();
        }
        emit dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, n);
    }

    void CIcaoDataReader::ps_parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CAirlineIcaoCodeList codes = CAirlineIcaoCodeList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockAirline);
            this->m_airlineIcaos = codes;
            n = codes.size();
        }
        emit dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, n);
    }

    void CIcaoDataReader::ps_parseCountryData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QJsonArray array = this->transformReplyIntoDatastoreResponse(nwReply.data());
        if (array.isEmpty())
        {
            emit dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CCountryList countries = CCountryList::fromDatabaseJson(array);

        // this part needs to be synchronized
        int n;
        {
            QWriteLocker wl(&this->m_lockCountry);
            this->m_countries = countries;
            n = m_countries.size();
        }
        emit dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, n);
    }

    bool CIcaoDataReader::canConnect(QString &message) const
    {
        if (m_urlAircraftIcao.isEmpty() || m_urlAirlineIcao.isEmpty()) { return false; }
        bool cm = CNetworkUtils::canConnect(m_urlAircraftIcao, message);

        // currently only testing one URL, might be changed in the future
        return cm;
    }

    QString CIcaoDataReader::getAircraftIcaoUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsonaircrafticao.php");
    }

    QString CIcaoDataReader::getAirlineIcaoUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsonairlineicao.php");
    }

    QString CIcaoDataReader::getCountryUrl(const QString &protocol, const QString &server, const QString &baseUrl)
    {
        return CNetworkUtils::buildUrl(protocol, server, baseUrl, "service/jsoncountry.php");
    }

} // namespace
