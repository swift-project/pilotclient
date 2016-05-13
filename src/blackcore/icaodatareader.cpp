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
#include "blackcore/icaodatareader.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"

#include <QDateTime>
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
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore
{
    CIcaoDataReader::CIcaoDataReader(QObject *owner) :
        CDatabaseReader(owner, "CIcaoDataReader")
    {
        // void
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

    CAircraftIcaoCode CIcaoDataReader::smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const
    {
        CAircraftIcaoCodeList codes(getAircraftIcaoCodes()); // thread safe copy
        return codes.smartAircraftIcaoSelector(icaoPattern); // sorted by rank
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

    CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodesForDesignator(const QString &designator) const
    {
        return getAirlineIcaoCodes().findByVDesignator(designator);
    }

    CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForDbKey(int key) const
    {
        return getAirlineIcaoCodes().findByKey(key);
    }

    CAirlineIcaoCode CIcaoDataReader::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern) const
    {
        CAirlineIcaoCodeList codes(this->getAirlineIcaoCodes()); // thread safe copy
        return codes.smartAirlineIcaoSelector(icaoPattern);
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

    void CIcaoDataReader::ps_read(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan)
    {
        this->threadAssertCheck(); // runs in background thread

        CEntityFlags::Entity entitiesTriggered = CEntityFlags::NoEntity;
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            CUrl url(getAircraftIcaoUrl());
            if (!url.isEmpty())
            {
                if (!newerThan.isNull()) { url.appendQuery("newer=" + newerThan.toString(Qt::ISODate)); }
                sApp->getFromNetwork(url, { this, &CIcaoDataReader::ps_parseAircraftIcaoData });
                entitiesTriggered |= CEntityFlags::AircraftIcaoEntity;
            }
            else
            {
                CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::AircraftIcaoEntity);
            }
        }

        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            CUrl url(getAirlineIcaoUrl());
            if (!url.isEmpty())
            {
                if (!newerThan.isNull()) { url.appendQuery("newer=" + newerThan.toString(Qt::ISODate)); }
                sApp->getFromNetwork(url, { this, &CIcaoDataReader::ps_parseAirlineIcaoData });
                entitiesTriggered |= CEntityFlags::AirlineIcaoEntity;
            }
            else
            {
                CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::AirlineIcaoEntity);
            }
        }

        if (entities.testFlag(CEntityFlags::CountryEntity))
        {
            CUrl url(getCountryUrl());
            if (!url.isEmpty())
            {
                if (!newerThan.isNull()) { url.appendQuery("newer=" + newerThan.toString(Qt::ISODate)); }
                sApp->getFromNetwork(url, { this, &CIcaoDataReader::ps_parseCountryData });
                entitiesTriggered |= CEntityFlags::CountryEntity;
            }
            else
            {
                CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::CountryEntity);
            }
        }

        if (entitiesTriggered != CEntityFlags::NoEntity)
        {
            emit dataRead(entitiesTriggered, CEntityFlags::StartRead, 0);
        }
    }

    CUrl CIcaoDataReader::getBaseUrl() const
    {
        const CUrl baseUrl(sApp->getGlobalSetup().getDbIcaoReaderUrl());
        return baseUrl;
    }

    void CIcaoDataReader::ps_parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QString urlString(nwReply->url().toString());
        CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CAircraftIcaoCodeList codes = CAircraftIcaoCodeList::fromDatabaseJson(res);

        // this part needs to be synchronized
        int n = codes.size();
        {
            QWriteLocker wl(&this->m_lockAircraft);
            this->m_aircraftIcaos = codes;
        }
        emit dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, n);
        CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::AircraftIcaoEntity) << urlString;
    }

    void CIcaoDataReader::ps_parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QString urlString(nwReply->url().toString());
        CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CAirlineIcaoCodeList codes = CAirlineIcaoCodeList::fromDatabaseJson(res);

        // this part needs to be synchronized
        int n = codes.size();
        {
            QWriteLocker wl(&this->m_lockAirline);
            this->m_airlineIcaos = codes;
        }
        emit dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, n);
        CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::AirlineIcaoEntity) << urlString;
    }

    void CIcaoDataReader::ps_parseCountryData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QString urlString(nwReply->url().toString());
        CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0);
            return;
        }
        CCountryList countries = CCountryList::fromDatabaseJson(res);

        // this part needs to be synchronized
        int n = m_countries.size();
        {
            QWriteLocker wl(&this->m_lockCountry);
            this->m_countries = countries;
        }
        emit dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, n);
        CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::CountryEntity) << urlString;
    }

    bool CIcaoDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead)
    {
        QDir directory(dir);
        if (!directory.exists()) { return false; }

        CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;
        if (whatToRead.testFlag(CEntityFlags::CountryEntity))
        {
            QString countriesJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "countries.json")));
            if (!countriesJson.isEmpty())
            {
                CCountryList countries;
                countries.convertFromJson(Json::jsonObjectFromString(countriesJson));
                int c = countries.size();
                {
                    QWriteLocker l(&m_lockCountry);
                    m_countries = countries;
                }
                // Do not emit while locked -> deadlock
                reallyRead |= CEntityFlags::CountryEntity;
                emit dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, c);
            }
        }

        if (whatToRead.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            QString aircraftJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "aircrafticao.json")));
            if (!aircraftJson.isEmpty())
            {
                CAircraftIcaoCodeList aircraftIcaos;
                aircraftIcaos.convertFromJson(Json::jsonObjectFromString(aircraftJson));
                int c = aircraftIcaos.size();
                {
                    QWriteLocker l(&m_lockAircraft);
                    m_aircraftIcaos = aircraftIcaos;
                }
                reallyRead |= CEntityFlags::AircraftIcaoEntity;
                emit dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, c);
            }
        }

        if (whatToRead.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            QString airlineJson(CFileUtils::readFileToString(CFileUtils::appendFilePaths(directory.absolutePath(), "airlineicao.json")));
            if (!airlineJson.isEmpty())
            {
                CAirlineIcaoCodeList airlineIcaos;
                airlineIcaos.convertFromJson(Json::jsonObjectFromString(airlineJson));
                int c = airlineIcaos.size();
                {
                    QWriteLocker l(&m_lockAirline);
                    m_airlineIcaos = airlineIcaos;
                }
                reallyRead |= CEntityFlags::AirlineIcaoEntity;
                emit dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c);
            }
        }
        return (whatToRead & CEntityFlags::AllIcaoAndCountries) == reallyRead;
    }

    bool CIcaoDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead)
    {
        if (dir.isEmpty() || whatToRead == CEntityFlags::NoEntity) { return false; }
        QTimer::singleShot(0, this, [this, dir, whatToRead]()
        {
            bool s = this->readFromJsonFiles(dir, whatToRead);
            Q_UNUSED(s);
        });
        return true;
    }

    bool CIcaoDataReader::writeToJsonFiles(const QString &dir) const
    {
        QDir directory(dir);
        if (!directory.exists()) { return false; }
        if (this->getCountriesCount() > 0)
        {
            QString json(QJsonDocument(this->getCountries().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "countries.json"));
            if (!s) { return false; }
        }

        if (this->getAircraftIcaoCodesCount() > 0)
        {
            QString json(QJsonDocument(this->getAircraftIcaoCodes().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "aircrafticao.json"));
            if (!s) { return false; }
        }

        if (this->getAirlineIcaoCodesCount() > 0)
        {
            QString json(QJsonDocument(this->getAirlineIcaoCodes().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "airlineicao.json"));
            if (!s) { return false; }
        }
        return true;
    }

    CUrl CIcaoDataReader::getAircraftIcaoUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsonaircrafticao.php");
    }

    CUrl CIcaoDataReader::getAirlineIcaoUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsonairlineicao.php");
    }

    CUrl CIcaoDataReader::getCountryUrl() const
    {
        return getBaseUrl().withAppendedPath("service/jsoncountry.php");
    }

} // namespace
