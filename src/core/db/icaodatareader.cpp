// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/db/icaodatareader.h"

#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QFlags>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QPointer>
#include <QReadLocker>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QStringBuilder>
#include <QTimer>
#include <QUrl>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>

#include "core/application.h"
#include "core/data/globalsetup.h"
#include "core/db/databaseutils.h"
#include "misc/aviation/aircraftcategorylist.h"
#include "misc/fileutils.h"
#include "misc/json.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::core::data;

namespace swift::core::db
{
    CIcaoDataReader::CIcaoDataReader(QObject *owner, const CDatabaseReaderConfigList &config)
        : CDatabaseReader(owner, config, "CIcaoDataReader")
    {
        // init to avoid threading issues
        this->getBaseUrl(CDbFlags::DbReading);
    }

    CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodes() const { return m_aircraftIcaoCache.get(); }

    CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDesignator(const QString &designator) const
    {
        return this->getAircraftIcaoCodes().findFirstByDesignatorAndRank(designator);
    }

    CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodesForDesignator(const QString &designator) const
    {
        return this->getAircraftIcaoCodes().findByDesignator(designator);
    }

    CAircraftIcaoCodeList CIcaoDataReader::getAircraftIcaoCodesForIataCode(const QString &iataCode) const
    {
        return this->getAircraftIcaoCodes().findByIataCode(iataCode);
    }

    CAircraftIcaoCode CIcaoDataReader::getAircraftIcaoCodeForDbKey(int key) const
    {
        return this->getAircraftIcaoCodes().findByKey(key);
    }

    bool CIcaoDataReader::containsAircraftIcaoDesignator(const QString &designator) const
    {
        return this->getAircraftIcaoCodes().containsDesignator(designator);
    }

    CAirlineIcaoCodeList CIcaoDataReader::getAirlineIcaoCodes() const { return m_airlineIcaoCache.get(); }

    CAircraftIcaoCode CIcaoDataReader::smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const
    {
        CAircraftIcaoCodeList codes(getAircraftIcaoCodes()); // thread safe copy
        return codes.smartAircraftIcaoSelector(icaoPattern); // sorted by rank
    }

    CCountryList CIcaoDataReader::getCountries() const { return m_countryCache.get(); }

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

    CAirlineIcaoCode CIcaoDataReader::getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator,
                                                                                     bool preferOperatingAirlines) const
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

    CAirlineIcaoCode CIcaoDataReader::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern,
                                                               const CCallsign &callsign) const
    {
        const CAirlineIcaoCodeList codes(this->getAirlineIcaoCodes()); // thread safe copy
        return codes.smartAirlineIcaoSelector(icaoPattern, callsign);
    }

    CAircraftCategoryList CIcaoDataReader::getAircraftCategories() const { return m_categoryCache.get(); }

    int CIcaoDataReader::getAircraftCategoryCount() const { return this->getAircraftCategories().size(); }

    int CIcaoDataReader::getAircraftIcaoCodesCount() const { return this->getAircraftIcaoCodes().size(); }

    int CIcaoDataReader::getAirlineIcaoCodesCount() const { return this->getAirlineIcaoCodes().size(); }

    bool CIcaoDataReader::areAllDataRead() const
    {
        return this->getCountriesCount() > 0 && getAirlineIcaoCodesCount() > 0 && getAircraftIcaoCodesCount() > 0;
    }

    int CIcaoDataReader::getCountriesCount() const { return this->getCountries().size(); }

    void CIcaoDataReader::read(CEntityFlags::Entity entities, CDbFlags::DataRetrievalModeFlag mode,
                               const QDateTime &newerThan)
    {
        this->threadAssertCheck(); // runs in background thread
        if (!this->doWorkCheck()) { return; }
        entities &= CEntityFlags::AllIcaoCountriesCategory;

        CEntityFlags::Entity entitiesTriggered = CEntityFlags::NoEntity;
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            CUrl url = this->getAircraftIcaoUrl(mode);
            if (!url.isEmpty())
            {
                url.appendQuery(queryLatestTimestamp(newerThan));
                this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::parseAircraftIcaoData });
                entitiesTriggered |= CEntityFlags::AircraftIcaoEntity;
            }
            else { this->logNoWorkingUrl(CEntityFlags::AircraftIcaoEntity); }
        }

        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            CUrl url = this->getAirlineIcaoUrl(mode);
            if (!url.isEmpty())
            {
                url.appendQuery(queryLatestTimestamp(newerThan));
                this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::parseAirlineIcaoData });
                entitiesTriggered |= CEntityFlags::AirlineIcaoEntity;
            }
            else { this->logNoWorkingUrl(CEntityFlags::AirlineIcaoEntity); }
        }

        if (entities.testFlag(CEntityFlags::CountryEntity))
        {
            CUrl url = this->getCountryUrl(mode);
            if (!url.isEmpty())
            {
                url.appendQuery(queryLatestTimestamp(newerThan));
                this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::parseCountryData });
                entitiesTriggered |= CEntityFlags::CountryEntity;
            }
            else { this->logNoWorkingUrl(CEntityFlags::CountryEntity); }
        }

        if (entities.testFlag(CEntityFlags::AircraftCategoryEntity))
        {
            CUrl url = this->getAircraftCategoryUrl(mode);
            if (!url.isEmpty())
            {
                url.appendQuery(queryLatestTimestamp(newerThan));
                this->getFromNetworkAndLog(url, { this, &CIcaoDataReader::parseAircraftCategoryData });
                entitiesTriggered |= CEntityFlags::AircraftCategoryEntity;
            }
            else { this->logNoWorkingUrl(CEntityFlags::AircraftCategoryEntity); }
        }

        if (entitiesTriggered != CEntityFlags::NoEntity)
        {
            emit this->dataRead(entitiesTriggered, CEntityFlags::ReadStarted, 0, getBaseUrl(mode));
        }
    }

    void CIcaoDataReader::aircraftIcaoCacheChanged() { this->cacheHasChanged(CEntityFlags::AircraftIcaoEntity); }

    void CIcaoDataReader::airlineIcaoCacheChanged() { this->cacheHasChanged(CEntityFlags::AirlineIcaoEntity); }

    void CIcaoDataReader::countryCacheChanged() { this->cacheHasChanged(CEntityFlags::CountryEntity); }

    void CIcaoDataReader::aircraftCategoryCacheChanged()
    {
        this->cacheHasChanged(CEntityFlags::AircraftCategoryEntity);
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
        if (m.isFailure()) { CLogMessage::preformatted(m); }
    }

    void CIcaoDataReader::parseAircraftIcaoData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (!this->doWorkCheck()) { return; }

        const CDatabaseReader::JsonDatastoreResponse res =
            this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        const QUrl url = nwReply->url();

        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0, url);
            return;
        }

        emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadParsing, 0, url);
        CAircraftIcaoCodeList codes;
        CAircraftIcaoCodeList inconsistent;
        const CAircraftCategoryList categories = this->getAircraftCategories();
        if (res.isRestricted())
        {
            // create full list if it was just incremental
            const CAircraftIcaoCodeList incrementalCodes(
                CAircraftIcaoCodeList::fromDatabaseJson(res, categories, true, &inconsistent));
            if (incrementalCodes.isEmpty()) { return; } // currently ignored
            codes = this->getAircraftIcaoCodes();
            codes.replaceOrAddObjectsByKey(incrementalCodes);
        }
        else
        {
            // normally read from special DB view which already filters incomplete
            QElapsedTimer time;
            time.start();
            codes = CAircraftIcaoCodeList::fromDatabaseJson(res, categories, true, &inconsistent);
            this->logParseMessage("aircraft ICAO", codes.size(), static_cast<int>(time.elapsed()), res);
        }

        if (!inconsistent.isEmpty())
        {
            logInconsistentData(CStatusMessage(this, CStatusMessage::SeverityInfo,
                                               u"Inconsistent aircraft codes: " % inconsistent.dbKeysAsString(", ")),
                                Q_FUNC_INFO);
        }

        if (!this->doWorkCheck()) { return; }
        const int n = codes.size();
        qint64 latestTimestamp = codes.latestTimestampMsecsSinceEpoch(); // ignores duplicates
        if (n > 0 && latestTimestamp < 0)
        {
            CLogMessage(this).error(u"No timestamp in aircraft ICAO list, setting to last modified value");
            latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
        }

        m_aircraftIcaoCache.set(codes, latestTimestamp);
        this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

        this->emitAndLogDataRead(CEntityFlags::AircraftIcaoEntity, n, res);
    }

    void CIcaoDataReader::parseAirlineIcaoData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (!this->doWorkCheck()) { return; }

        const QUrl url = nwReply->url();
        const CDatabaseReader::JsonDatastoreResponse res =
            this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0, url);
            return;
        }

        emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadParsing, 0, url);
        CAirlineIcaoCodeList codes;
        CAirlineIcaoCodeList inconsistent;
        if (res.isRestricted())
        {
            // create full list if it was just incremental
            const CAirlineIcaoCodeList incrementalCodes(
                CAirlineIcaoCodeList::fromDatabaseJson(res, true, &inconsistent));
            if (incrementalCodes.isEmpty()) { return; } // currently ignored
            codes = this->getAirlineIcaoCodes();
            codes.replaceOrAddObjectsByKey(incrementalCodes);
        }
        else
        {
            // normally read from special DB view which already filters incomplete
            QElapsedTimer time;
            time.start();
            codes = CAirlineIcaoCodeList::fromDatabaseJson(res, true, &inconsistent);
            this->logParseMessage("airline ICAO", codes.size(), static_cast<int>(time.elapsed()), res);
        }

        if (!inconsistent.isEmpty())
        {
            logInconsistentData(CStatusMessage(this, CStatusMessage::SeverityInfo,
                                               u"Inconsistent airline codes: " % inconsistent.dbKeysAsString(", ")),
                                Q_FUNC_INFO);
        }

        if (!this->doWorkCheck()) { return; }
        const int n = codes.size();
        qint64 latestTimestamp = codes.latestTimestampMsecsSinceEpoch();
        if (n > 0 && latestTimestamp < 0)
        {
            CLogMessage(this).error(u"No timestamp in airline ICAO list, setting to last modified value");
            latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
        }

        m_airlineIcaoCache.set(codes, latestTimestamp);
        this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

        this->emitAndLogDataRead(CEntityFlags::AirlineIcaoEntity, n, res);
    }

    void CIcaoDataReader::parseCountryData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        const CDatabaseReader::JsonDatastoreResponse res =
            this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        const QUrl url = nwReply->url();

        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0, url);
            return;
        }

        emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadParsing, 0, url);
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
            // normally read from special DB view which already filters incomplete
            QElapsedTimer time;
            time.start();
            countries = CCountryList::fromDatabaseJson(res);
            this->logParseMessage("countries", countries.size(), static_cast<int>(time.elapsed()), res);
        }

        if (!this->doWorkCheck()) { return; }
        const int n = countries.size();
        qint64 latestTimestamp = countries.latestTimestampMsecsSinceEpoch();
        if (n > 0 && latestTimestamp < 0)
        {
            CLogMessage(this).error(u"No timestamp in country list, setting to last modified value");
            latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
        }

        m_countryCache.set(countries, latestTimestamp);
        this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

        this->emitAndLogDataRead(CEntityFlags::CountryEntity, n, res);
    }

    void CIcaoDataReader::parseAircraftCategoryData(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        const CDatabaseReader::JsonDatastoreResponse res =
            this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        const QUrl url = nwReply->url();

        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadFailed, 0, url);
            return;
        }

        emit this->dataRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadParsing, 0, url);
        CAircraftCategoryList categories;
        if (res.isRestricted())
        {
            // create full list if it was just incremental
            const CAircraftCategoryList incrementalCategories(CAircraftCategoryList::fromDatabaseJson(res));
            if (incrementalCategories.isEmpty()) { return; } // currently ignored
            categories = this->getAircraftCategories();
            categories.replaceOrAddObjectsByKey(incrementalCategories);
        }
        else
        {
            // normally read from special DB view which already filters incomplete
            QElapsedTimer time;
            time.start();
            categories = CAircraftCategoryList::fromDatabaseJson(res);
            this->logParseMessage("categories", categories.size(), static_cast<int>(time.elapsed()), res);
        }

        if (!this->doWorkCheck()) { return; }
        const int n = categories.size();
        qint64 latestTimestamp = categories.latestTimestampMsecsSinceEpoch();
        if (n > 0 && latestTimestamp < 0)
        {
            CLogMessage(this).error(u"No timestamp in category list, setting to last modified value");
            latestTimestamp = this->lastModifiedMsSinceEpoch(nwReply.data());
        }

        m_categoryCache.set(categories, latestTimestamp);
        this->updateReaderUrl(this->getBaseUrl(CDbFlags::DbReading));

        this->emitAndLogDataRead(CEntityFlags::AircraftCategoryEntity, n, res);
    }

    CStatusMessageList CIcaoDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead,
                                                          bool overrideNewerOnly)
    {
        const QDir directory(dir);
        if (!directory.exists()) { return CStatusMessage(this).error(u"Missing directory '%1'") << dir; }

        // Hint: Do not emit while locked -> deadlock
        CStatusMessageList msgs;
        whatToRead &= CEntityFlags::AllIcaoCountriesCategory;
        CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;
        if (whatToRead.testFlag(CEntityFlags::CountryEntity))
        {
            const QString fileName = CFileUtils::appendFilePaths(
                directory.absolutePath(), CDbInfo::entityToSharedName(CEntityFlags::CountryEntity));
            const QFileInfo fi(fileName);
            if (!fi.exists()) { msgs.push_back(CStatusMessage(this).warning(u"File '%1' does not exist") << fileName); }
            else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::CountryEntity, msgs))
            {
                // void
            }
            else
            {
                const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
                const QJsonObject countriesJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                if (countriesJson.isEmpty())
                {
                    msgs.push_back(CStatusMessage(this).error(u"Failed to read from file/empty file '%1'") << fileName);
                }
                else
                {
                    try
                    {
                        const CCountryList countries = CCountryList::fromMultipleJsonFormats(countriesJson);
                        const int c = countries.size();
                        msgs.push_back(m_countryCache.set(countries, fi.birthTime().toUTC().toMSecsSinceEpoch()));
                        reallyRead |= CEntityFlags::CountryEntity;
                        emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, c, url);
                    }
                    catch (const CJsonException &ex)
                    {
                        emit this->dataRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFailed, 0, url);
                        msgs.push_back(CStatusMessage::fromJsonException(
                            ex, this, QStringLiteral("Reading countries from '%1'").arg(fileName)));
                    }
                }
            }
        } // country

        if (whatToRead.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            const QString fileName = CFileUtils::appendFilePaths(
                directory.absolutePath(), CDbInfo::entityToSharedName(CEntityFlags::AircraftIcaoEntity));
            const QFileInfo fi(fileName);
            if (!fi.exists()) { msgs.push_back(CStatusMessage(this).warning(u"File '%1' does not exist") << fileName); }
            else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AircraftIcaoEntity, msgs))
            {
                // void
            }
            else
            {
                const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
                const QJsonObject aircraftJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                if (aircraftJson.isEmpty())
                {
                    msgs.push_back(CStatusMessage(this).error(u"Failed to read from file/empty file '%1'") << fileName);
                }
                else
                {
                    try
                    {
                        const CAircraftIcaoCodeList aircraftIcaos =
                            CAircraftIcaoCodeList::fromMultipleJsonFormats(aircraftJson);
                        const int c = aircraftIcaos.size();
                        msgs.push_back(
                            m_aircraftIcaoCache.set(aircraftIcaos, fi.birthTime().toUTC().toMSecsSinceEpoch()));
                        reallyRead |= CEntityFlags::AircraftIcaoEntity;
                        emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, c, url);
                    }
                    catch (const CJsonException &ex)
                    {
                        emit this->dataRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFailed, 0, url);
                        msgs.push_back(CStatusMessage::fromJsonException(
                            ex, this, QStringLiteral("Reading aircraft ICAOs from '%1'").arg(fileName)));
                    }
                }
            }
        } // aircraft

        if (whatToRead.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            const QString fileName = CFileUtils::appendFilePaths(
                directory.absolutePath(), CDbInfo::entityToSharedName(CEntityFlags::AirlineIcaoEntity));
            const QFileInfo fi(fileName);
            if (!fi.exists()) { msgs.push_back(CStatusMessage(this).warning(u"File '%1' does not exist") << fileName); }
            else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AirlineIcaoEntity, msgs))
            {
                // void
            }
            else
            {
                const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
                const QJsonObject airlineJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                if (airlineJson.isEmpty())
                {
                    msgs.push_back(CStatusMessage(this).error(u"Failed to read from file/empty file '%1'") << fileName);
                }
                else
                {
                    try
                    {
                        const CAirlineIcaoCodeList airlineIcaos =
                            CAirlineIcaoCodeList::fromMultipleJsonFormats(airlineJson);
                        const int c = airlineIcaos.size();
                        msgs.push_back(
                            m_airlineIcaoCache.set(airlineIcaos, fi.birthTime().toUTC().toMSecsSinceEpoch()));
                        reallyRead |= CEntityFlags::AirlineIcaoEntity;
                        emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c, url);
                    }
                    catch (const CJsonException &ex)
                    {
                        emit this->dataRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFailed, 0, url);
                        msgs.push_back(CStatusMessage::fromJsonException(
                            ex, this, QStringLiteral("Reading airline ICAOs from '%1'").arg(fileName)));
                    }
                }
            }
        } // airline

        if (whatToRead.testFlag(CEntityFlags::AircraftCategoryEntity))
        {
            const QString fileName = CFileUtils::appendFilePaths(
                directory.absolutePath(), CDbInfo::entityToSharedName(CEntityFlags::AircraftCategoryEntity));
            const QFileInfo fi(fileName);
            if (!fi.exists()) { msgs.push_back(CStatusMessage(this).warning(u"File '%1' does not exist") << fileName); }
            else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AircraftCategoryEntity, msgs))
            {
                // void
            }
            else
            {
                const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
                const QJsonObject aircraftCategory(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
                if (aircraftCategory.isEmpty())
                {
                    msgs.push_back(CStatusMessage(this).error(u"Failed to read from file/empty file '%1'") << fileName);
                }
                else
                {
                    try
                    {
                        const CAircraftCategoryList aircraftCategories =
                            CAircraftCategoryList::fromMultipleJsonFormats(aircraftCategory);
                        const int c = aircraftCategories.size();
                        msgs.push_back(
                            m_categoryCache.set(aircraftCategories, fi.birthTime().toUTC().toMSecsSinceEpoch()));
                        reallyRead |= CEntityFlags::AircraftCategoryEntity;
                        emit this->dataRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadFinished, c, url);
                    }
                    catch (const CJsonException &ex)
                    {
                        emit this->dataRead(CEntityFlags::AircraftCategoryEntity, CEntityFlags::ReadFailed, 0, url);
                        msgs.push_back(CStatusMessage::fromJsonException(
                            ex, this, QStringLiteral("Reading categories from '%1'").arg(fileName)));
                    }
                }
            }
        } // categories

        return msgs;
    }

    bool CIcaoDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead,
                                                        bool overrideNewerOnly)
    {
        if (dir.isEmpty() || whatToRead == CEntityFlags::NoEntity) { return false; }

        QPointer<CIcaoDataReader> myself(this);
        QTimer::singleShot(0, this, [=]() {
            if (!myself) { return; }
            const CStatusMessageList msgs = this->readFromJsonFiles(dir, whatToRead, overrideNewerOnly);
            if (msgs.isFailure()) { CLogMessage::preformatted(msgs); }
        });
        return true;
    }

    bool CIcaoDataReader::writeToJsonFiles(const QString &dir)
    {
        QDir directory(dir);
        if (!directory.exists()) { return false; }
        QList<QPair<CEntityFlags::EntityFlag, QString>> fileContents;
        if (this->getCountriesCount() > 0)
        {
            const QString json(QJsonDocument(this->getCountries().toJson()).toJson());
            fileContents.push_back({ CEntityFlags::CountryEntity, json });
        }

        if (this->getAircraftIcaoCodesCount() > 0)
        {
            const QString json(QJsonDocument(this->getAircraftIcaoCodes().toJson()).toJson());
            fileContents.push_back({ CEntityFlags::AircraftIcaoEntity, json });
        }

        if (this->getAirlineIcaoCodesCount() > 0)
        {
            const QString json(QJsonDocument(this->getAirlineIcaoCodes().toJson()).toJson());
            fileContents.push_back({ CEntityFlags::AirlineIcaoEntity, json });
        }

        if (this->getAircraftCategoryCount() > 0)
        {
            const QString json(QJsonDocument(this->getAirlineIcaoCodes().toJson()).toJson());
            fileContents.push_back({ CEntityFlags::AircraftCategoryEntity, json });
        }

        for (const auto &pair : fileContents)
        {
            CWorker::fromTask(this, Q_FUNC_INFO, [pair, directory] {
                CFileUtils::writeStringToFile(
                    CFileUtils::appendFilePaths(directory.absolutePath(), CDbInfo::entityToSharedName(pair.first)),
                    pair.second);
            });
        }
        return true;
    }

    CEntityFlags::Entity CIcaoDataReader::getSupportedEntities() const
    {
        return CEntityFlags::AllIcaoCountriesCategory;
    }

    void CIcaoDataReader::synchronizeCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            if (m_syncedAircraftIcaoCache) { return; }
            m_syncedAircraftIcaoCache = true;
            m_aircraftIcaoCache.synchronize();
        }
        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            if (m_syncedAirlineIcaoCache) { return; }
            m_syncedAirlineIcaoCache = true;
            m_airlineIcaoCache.synchronize();
        }
        if (entities.testFlag(CEntityFlags::CountryEntity))
        {
            if (m_syncedCountryCache) { return; }
            m_syncedCountryCache = true;
            m_countryCache.synchronize();
        }
        if (entities.testFlag(CEntityFlags::AircraftCategoryEntity))
        {
            if (m_syncedCategories) { return; }
            m_syncedCategories = true;
            m_categoryCache.synchronize();
        }
    }

    void CIcaoDataReader::admitCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity)) { m_aircraftIcaoCache.admit(); }
        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity)) { m_airlineIcaoCache.admit(); }
        if (entities.testFlag(CEntityFlags::CountryEntity)) { m_countryCache.admit(); }
        if (entities.testFlag(CEntityFlags::AircraftCategoryEntity)) { m_categoryCache.admit(); }
    }

    void CIcaoDataReader::invalidateCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AircraftIcaoEntity))
        {
            CDataCache::instance()->clearAllValues(m_aircraftIcaoCache.getKey());
        }
        if (entities.testFlag(CEntityFlags::AirlineIcaoEntity))
        {
            CDataCache::instance()->clearAllValues(m_airlineIcaoCache.getKey());
        }
        if (entities.testFlag(CEntityFlags::CountryEntity))
        {
            CDataCache::instance()->clearAllValues(m_countryCache.getKey());
        }
        if (entities.testFlag(CEntityFlags::AircraftCategoryEntity))
        {
            CDataCache::instance()->clearAllValues(m_categoryCache.getKey());
        }
    }

    QDateTime CIcaoDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
    {
        switch (entity)
        {
        case CEntityFlags::AircraftIcaoEntity: return m_aircraftIcaoCache.getAvailableTimestamp();
        case CEntityFlags::AirlineIcaoEntity: return m_airlineIcaoCache.getAvailableTimestamp();
        case CEntityFlags::CountryEntity: return m_countryCache.getAvailableTimestamp();
        case CEntityFlags::AircraftCategoryEntity: return m_categoryCache.getAvailableTimestamp();
        default: return QDateTime();
        }
    }

    int CIcaoDataReader::getCacheCount(CEntityFlags::Entity entity) const
    {
        switch (entity)
        {
        case CEntityFlags::AircraftIcaoEntity: return m_aircraftIcaoCache.get().size();
        case CEntityFlags::AirlineIcaoEntity: return m_airlineIcaoCache.get().size();
        case CEntityFlags::CountryEntity: return m_countryCache.get().size();
        case CEntityFlags::AircraftCategoryEntity: return m_categoryCache.get().size();
        default: return 0;
        }
    }

    CEntityFlags::Entity CIcaoDataReader::getEntitiesWithCacheCount() const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (this->getCacheCount(CEntityFlags::AircraftIcaoEntity) > 0) entities |= CEntityFlags::AircraftIcaoEntity;
        if (this->getCacheCount(CEntityFlags::AirlineIcaoEntity) > 0) entities |= CEntityFlags::AirlineIcaoEntity;
        if (this->getCacheCount(CEntityFlags::CountryEntity) > 0) entities |= CEntityFlags::CountryEntity;
        if (this->getCacheCount(CEntityFlags::AircraftCategoryEntity) > 0)
            entities |= CEntityFlags::AircraftCategoryEntity;
        return entities;
    }

    CEntityFlags::Entity CIcaoDataReader::getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (this->hasCacheTimestampNewerThan(CEntityFlags::AircraftIcaoEntity, threshold))
            entities |= CEntityFlags::AircraftIcaoEntity;
        if (this->hasCacheTimestampNewerThan(CEntityFlags::AirlineIcaoEntity, threshold))
            entities |= CEntityFlags::AirlineIcaoEntity;
        if (this->hasCacheTimestampNewerThan(CEntityFlags::CountryEntity, threshold))
            entities |= CEntityFlags::CountryEntity;
        if (this->hasCacheTimestampNewerThan(CEntityFlags::AircraftCategoryEntity, threshold))
            entities |= CEntityFlags::CountryEntity;
        return entities;
    }

    bool CIcaoDataReader::hasChangedUrl(CEntityFlags::Entity entity, CUrl &oldUrlInfo, CUrl &newUrlInfo) const
    {
        Q_UNUSED(entity)
        oldUrlInfo = m_readerUrlCache.get();
        newUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
        return CDatabaseReader::isChangedUrl(oldUrlInfo, newUrlInfo);
    }

    CUrl CIcaoDataReader::getDbServiceBaseUrl() const { return sApp->getGlobalSetup().getDbIcaoReaderUrl(); }

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

    CUrl CIcaoDataReader::getAircraftCategoryUrl(CDbFlags::DataRetrievalModeFlag mode) const
    {
        return this->getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::AircraftCategoryEntity, mode));
    }
} // namespace swift::core::db
