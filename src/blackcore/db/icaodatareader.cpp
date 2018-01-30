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
#include "blackcore/db/icaodatareader.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"

#include <QDateTime>
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
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Db
    {
        CIcaoDataReader::CIcaoDataReader(QObject *owner, const CDatabaseReaderConfigList &config) :
            CDatabaseReader(owner, config, "CIcaoDataReader")
        {
            // init to avoid threading issues
            this->getBaseUrl(CDbFlags::DbReading);
        }

        CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodes() const
        {
            return m_aircraftIcaoCache.get();
        }

        CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDesignator(const QString &designator) const
        {
            return getAircraftIcaoCodes().findFirstByDesignatorAndRank(designator);
        }

        CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodesForDesignator(const QString &designator) const
        {
            return getAircraftIcaoCodes().findByDesignator(designator);
        }

        CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodesForIataCode(const QString &iataCode) const
        {
            return getAircraftIcaoCodes().findByIataCode(iataCode);
        }

        CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDbKey(int key) const
        {
            return getAircraftIcaoCodes().findByKey(key);
        }

        bool CIcaoDataReader::containsAircraftIcaoDesignator(const QString &designator) const
        {
            return this->getAircraftIcaoCodes().containsDesignator(designator);
        }

        CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodes() const
        {
            return m_airlineIcaoCache.get();
        }

        CAircraftIcaoCode CIcaoDataReader::smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const
        {
            CAircraftIcaoCodeList codes(getAircraftIcaoCodes()); // thread safe copy
            return codes.smartAircraftIcaoSelector(icaoPattern); // sorted by rank
        }

        CCountryList CIcaoDataReader::getCountries() const
        {
            return m_countryCache.get();
        }

        CCountry CIcaoDataReader::getCountryForIsoCode(const QString &isoCode) const
        {
            return this->getCountries().findByIsoCode(isoCode);
        }

        CCountry CIcaoDataReader::getCountryForName(const QString &name) const
        {
            return this->getCountries().findBestMatchByCountryName(name);
        }

        CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodesForDesignator(const QString &designator) const
        {
            return this->getAirlineIcaoCodes().findByVDesignator(designator);
        }

        bool CIcaoDataReader::containsAirlineIcaoDesignator(const QString &designator) const
        {
            return this->getAirlineIcaoCodes().containsVDesignator(designator);
        }

        CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const
        {
            return this->getAirlineIcaoCodes().findByUniqueVDesignatorOrDefault(designator, preferOperatingAirlines);
        }

        CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodesForIataCode(const QString &iataCode) const
        {
            return this->getAirlineIcaoCodes().findByIataCode(iataCode);
        }

        CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const
        {
            return this->getAirlineIcaoCodes().findByUniqueIataCodeOrDefault(iataCode);
        }

        CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForDbKey(int key) const
        {
            return this->getAirlineIcaoCodes().findByKey(key);
        }

        CAirlineIcaoCode CIcaoDataReader::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign) const
        {
            const CAirlineIcaoCodeList codes(this->getAirlineIcaoCodes()); // thread safe copy
            return codes.smartAirlineIcaoSelector(icaoPattern, callsign);
        }

        int CIcaoDataReader::getAircraftIcaoCodesCount() const
        {
            return this->getAircraftIcaoCodes().size();
        }

        int CIcaoDataReader::getAirlineIcaoCodesCount() const
        {
            return this->getAirlineIcaoCodes().size();
        }

        bool CIcaoDataReader::areAllDataRead() const
        {
            return getCountriesCount() > 0 && getAirlineIcaoCodesCount() > 0 && getAircraftIcaoCodesCount() > 0;
        }

        int CIcaoDataReader::getCountriesCount() const
        {
            return this->getCountries().size();
        }

        void CIcaoDataReader::ps_read(CEntityFlags::Entity entities, CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan)
        {
            this->threadAssertCheck(); // runs in background thread
            if (!this->doWorkCheck()) { return; }
            entities &= CEntityFlags::AllIcaoAndCountries;
            if (!this->isInternetAccessible())
            {
                emit this->dataRead(entities, CEntityFlags::ReadSkipped, 0);
                return;
            }

            CEntityFlags::Entity entitiesTriggered = CEntityFlags::NoEntity;
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
            {
                CUrl url(getAircraftIcaoUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::ps_parseAircraftIcaoData });
                    entitiesTriggered |= CEntityFlags::AircraftIcaoEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::AircraftIcaoEntity);
                }
            }

            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
            {
                CUrl url(getAirlineIcaoUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::ps_parseAirlineIcaoData });
                    entitiesTriggered |= CEntityFlags::AirlineIcaoEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::AirlineIcaoEntity);
                }
            }

            if (entities.testFlag(CEntityFlags::CountryEntity))
            {
                CUrl url(getCountryUrl(mode));
                if (!url.isEmpty())
                {
                    url.appendQuery(queryLatestTimestamp(newerThan));
                    this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::ps_parseCountryData });
                    entitiesTriggered |= CEntityFlags::CountryEntity;
                }
                else
                {
                    this->logNoWorkingUrl(CEntityFlags::CountryEntity);
                }
            }

            if (entitiesTriggered != CEntityFlags::NoEntity)
            {
                emit this->dataRead(entitiesTriggered, CEntityFlags::StartRead, 0);
            }
        }

        void CIcaoDataReader::aircraftIcaoCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::AircraftIcaoEntity);
        }

        void CIcaoDataReader::airlineIcaoCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::AirlineIcaoEntity);
        }

        void CIcaoDataReader::countryCacheChanged()
        {
            this->cacheHasChanged(CEntityFlags::CountryEntity);
        }

        void CIcaoDataReader::baseUrlCacheChanged()
        {
            // void
        }

        void CIcaoDataReader::updateReaderUrl(const CUrl &url)
        {
            const CUrl current = m_readerUrlCache.get();
            if (current == url) { return; }
            const CStatusMessage m = m_readerUrlCache.set(url);
            if (m.isFailure())
            {
                CLogMessage::preformatted(m);
            }
        }

        void CIcaoDataReader::ps_parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (!this->doWorkCheck()) { return; }

            const CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            CAircraftIcaoCodeList codes;
            CAircraftIcaoCodeList inconsistent;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                const CAircraftIcaoCodeList incrementalCodes(CAircraftIcaoCodeList::fromDatabaseJson(res, true, &inconsistent));
                if (incrementalCodes.isEmpty()) { return; } // currently ignored
                codes = this->getAircraftIcaoCodes();
                codes.replaceOrAddObjectsByKey(incrementalCodes);
            }
            else
            {
                // normally read from special view which already filters incomplete
                codes  = CAircraftIcaoCodeList::fromDatabaseJson(res, true, &inconsistent);
            }

            if (!inconsistent.isEmpty())
            {
                logInconsistentData(
                    CStatusMessage(this, CStatusMessage::SeverityInfo, "Inconsistent aircraft codes: " + inconsistent.dbKeysAsString(", ")),
                    Q_FUNC_INFO);
            }

            const int n = codes.size();
            qint64 latestTimestamp = codes.latestTimestampMsecsSinceEpoch(); // ignores duplicates
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in aircraft ICAO list, setting to last modified value");
                latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
            }

            m_aircraftIcaoCache.set(codes, latestTimestamp);
            this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::AircraftIcaoEntity, n, res);
        }

        void CIcaoDataReader::ps_parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
        {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (!this->doWorkCheck()) { return; }

            const CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            CAirlineIcaoCodeList codes;
            CAirlineIcaoCodeList inconsistent;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                const CAirlineIcaoCodeList incrementalCodes(CAirlineIcaoCodeList::fromDatabaseJson(res, true, &inconsistent));
                if (incrementalCodes.isEmpty()) { return; } // currently ignored
                codes = this->getAirlineIcaoCodes();
                codes.replaceOrAddObjectsByKey(incrementalCodes);
            }
            else
            {
                // normally read from special view which already filters incomplete
                codes = CAirlineIcaoCodeList::fromDatabaseJson(res, true, &inconsistent);
            }

            if (!inconsistent.isEmpty())
            {
                logInconsistentData(
                    CStatusMessage(this, CStatusMessage::SeverityInfo, "Inconsistent airline codes: " + inconsistent.dbKeysAsString(", ")),
                    Q_FUNC_INFO);
            }

            const int n = codes.size();
            qint64 latestTimestamp = codes.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in airline ICAO list, setting to last modified value");
                latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
            }

            m_airlineIcaoCache.set(codes, latestTimestamp);
            this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::AirlineIcaoEntity, n, res);
        }

        void CIcaoDataReader::ps_parseCountryData(QNetworkReply *nwReplyPtr)
        {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            const CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            CCountryList countries;
            if (res.isRestricted())
            {
                // create full list if it was just incremental
                const CCountryList incrementalCountries(CCountryList::fromDatabaseJson(res));
                if (incrementalCountries.isEmpty()) { return; } // currently ignored
                countries = this->getCountries();
                countries.replaceOrAddObjectsByKey(incrementalCountries);
            }
            else
            {
                // normally read from special view which already filters incomplete
                countries  = CCountryList::fromDatabaseJson(res);
            }

            const int n = countries.size();
            qint64 latestTimestamp = countries.latestTimestampMsecsSinceEpoch();
            if (n > 0 && latestTimestamp < 0)
            {
                CLogMessage(this).error("No timestamp in country list, setting to last modified value");
                latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
            }

            m_countryCache.set(countries, latestTimestamp);
            this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

            this->emitAndLogDataRead(CEntityFlags::CountryEntity, n, res);
        }

        CStatusMessageList CIcaoDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
        {
            const QDir directory(dir);
            if (!directory.exists())
            {
                return CStatusMessage(this).error("Missing directory '%1'") << dir;
            }

            // Hint: Do not emit while locked -> deadlock
            CStatusMessageList msgs;
            whatToRead &= CEntityFlags::AllIcaoAndCountries;
            CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;
            if (whatToRead.testFlag(CEntityFlags::CountryEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "countries.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::CountryEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject countriesJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (countriesJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CCountryList countries = CCountryList::fromMultipleJsonFormats(countriesJson);
                            const int c = countries.size();
                            msgs.push_back(m_countryCache.set(countries, fi.created().toUTC().toMSecsSinceEpoch()));
                            reallyRead |= CEntityFlags::CountryEntity;
                            emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, c);
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading countries from '%1'").arg(fileName)));
                        }
                    }
                }
            } // country

            if (whatToRead.testFlag(CEntityFlags::AircraftIcaoEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "aircrafticao.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AircraftIcaoEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject aircraftJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (aircraftJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CAircraftIcaoCodeList aircraftIcaos = CAircraftIcaoCodeList::fromMultipleJsonFormats(aircraftJson);
                            const int c = aircraftIcaos.size();
                            msgs.push_back(m_aircraftIcaoCache.set(aircraftIcaos, fi.created().toUTC().toMSecsSinceEpoch()));
                            reallyRead |= CEntityFlags::AircraftIcaoEntity;
                            emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, c);
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading aircraft ICAOs from '%1'").arg(fileName)));
                        }
                    }
                }
            } // aircraft

            if (whatToRead.testFlag(CEntityFlags::AirlineIcaoEntity))
            {
                const QString fileName = CFileUtils::appendFilePaths(directory.absolutePath(), "airlineicao.json");
                const QFileInfo fi(fileName);
                if (!fi.exists())
                {
                    msgs.push_back(CStatusMessage(this).warning("File '%1' does not exist") << fileName);
                }
                else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AirlineIcaoEntity, msgs))
                {
                    // void
                }
                else
                {
                    const QJsonObject airlineJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                    if (airlineJson.isEmpty())
                    {
                        msgs.push_back(CStatusMessage(this).error("Failed to read from file/empty file '%1'") << fileName);
                    }
                    else
                    {
                        try
                        {
                            const CAirlineIcaoCodeList airlineIcaos = CAirlineIcaoCodeList::fromMultipleJsonFormats(airlineJson);
                            const int c = airlineIcaos.size();
                            msgs.push_back(m_airlineIcaoCache.set(airlineIcaos, fi.created().toUTC().toMSecsSinceEpoch()));
                            reallyRead |= CEntityFlags::AirlineIcaoEntity;
                            emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c);
                        }
                        catch (const CJsonException &ex)
                        {
                            emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0);
                            msgs.push_back(ex.toStatusMessage(this, QString("Reading airline ICAOs from '%1'").arg(fileName)));
                        }
                    }
                }
            } // airline

            return msgs;
        }

        bool CIcaoDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
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

        bool CIcaoDataReader::writeToJsonFiles(const QString &dir) const
        {
            QDir directory(dir);
            if (!directory.exists()) { return false; }
            if (this->getCountriesCount() > 0)
            {
                const QString json(QJsonDocument(this->getCountries().toJson()).toJson());
                const bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "countries.json"));
                if (!s) { return false; }
            }

            if (this->getAircraftIcaoCodesCount() > 0)
            {
                const QString json(QJsonDocument(this->getAircraftIcaoCodes().toJson()).toJson());
                const bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "aircrafticao.json"));
                if (!s) { return false; }
            }

            if (this->getAirlineIcaoCodesCount() > 0)
            {
                const QString json(QJsonDocument(this->getAirlineIcaoCodes().toJson()).toJson());
                const bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "airlineicao.json"));
                if (!s) { return false; }
            }
            return true;
        }

        CEntityFlags::Entity CIcaoDataReader::getSupportedEntities() const
        {
            return CEntityFlags::AllIcaoAndCountries;
        }

        void CIcaoDataReader::synchronizeCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity)) { m_aircraftIcaoCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity)) { m_airlineIcaoCache.synchronize(); }
            if (entities.testFlag(CEntityFlags::CountryEntity)) { m_countryCache.synchronize(); }
        }

        void CIcaoDataReader::admitCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity)) { m_aircraftIcaoCache.admit(); }
            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity)) { m_airlineIcaoCache.admit(); }
            if (entities.testFlag(CEntityFlags::CountryEntity)) { m_countryCache.admit(); }
        }

        void CIcaoDataReader::invalidateCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity)) { CDataCache::instance()->clearAllValues(m_aircraftIcaoCache.getKey()); }
            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity)) {CDataCache::instance()->clearAllValues(m_airlineIcaoCache.getKey()); }
            if (entities.testFlag(CEntityFlags::CountryEntity)) {CDataCache::instance()->clearAllValues(m_countryCache.getKey()); }
        }

        QDateTime CIcaoDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::AircraftIcaoEntity: return m_aircraftIcaoCache.getAvailableTimestamp();
            case CEntityFlags::AirlineIcaoEntity:  return m_airlineIcaoCache.getAvailableTimestamp();
            case CEntityFlags::CountryEntity:      return m_countryCache.getAvailableTimestamp();
            default: return QDateTime();
            }
        }

        int CIcaoDataReader::getCacheCount(CEntityFlags::Entity entity) const
        {
            switch (entity)
            {
            case CEntityFlags::AircraftIcaoEntity: return m_aircraftIcaoCache.get().size();
            case CEntityFlags::AirlineIcaoEntity:  return m_airlineIcaoCache.get().size();
            case CEntityFlags::CountryEntity:      return m_countryCache.get().size();
            default: return 0;
            }
        }

        CEntityFlags::Entity CIcaoDataReader::getEntitiesWithCacheCount() const
        {
            CEntityFlags::Entity entities = CEntityFlags::NoEntity;
            if (this->getCacheCount(CEntityFlags::AircraftIcaoEntity) > 0) entities |= CEntityFlags::AircraftIcaoEntity;
            if (this->getCacheCount(CEntityFlags::AirlineIcaoEntity) > 0) entities |= CEntityFlags::AirlineIcaoEntity;
            if (this->getCacheCount(CEntityFlags::CountryEntity) > 0) entities |= CEntityFlags::CountryEntity;
            return entities;
        }

        CEntityFlags::Entity CIcaoDataReader::getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const
        {
            CEntityFlags::Entity entities = CEntityFlags::NoEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::AircraftIcaoEntity, threshold)) entities |= CEntityFlags::AircraftIcaoEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::AirlineIcaoEntity, threshold)) entities |= CEntityFlags::AirlineIcaoEntity;
            if (this->hasCacheTimestampNewerThan(CEntityFlags::CountryEntity, threshold)) entities |= CEntityFlags::CountryEntity;
            return entities;
        }

        bool CIcaoDataReader::hasChangedUrl(CEntityFlags::Entity entity, CUrl &oldUrlInfo, CUrl &newUrlInfo) const
        {
            Q_UNUSED(entity);
            oldUrlInfo = m_readerUrlCache.get();
            newUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
            return CDatabaseReader::isChangedUrl(oldUrlInfo, newUrlInfo);
        }

        CUrl CIcaoDataReader::getDbServiceBaseUrl() const
        {
            return sApp->getGlobalSetup().getDbIcaoReaderUrl();
        }

        CUrl CIcaoDataReader::getAircraftIcaoUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return this->getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::AircraftIcaoEntity, mode));
        }

        CUrl CIcaoDataReader::getAirlineIcaoUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return this->getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::AirlineIcaoEntity, mode));
        }

        CUrl CIcaoDataReader::getCountryUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            return this->getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::CountryEntity, mode));
        }
    } // ns
} // ns
