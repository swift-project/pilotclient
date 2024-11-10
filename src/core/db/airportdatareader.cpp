// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/db/airportdatareader.h"
#include "core/db/databaseutils.h"
#include "core/application.h"
#include "misc/network/networkutils.h"
#include "misc/logmessage.h"

#include <QStringBuilder>
#include <QNetworkReply>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QPointer>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::db;

namespace swift::core::db
{
    CAirportDataReader::CAirportDataReader(QObject *parent, const CDatabaseReaderConfigList &config) : CDatabaseReader(parent, config, QStringLiteral("CAirportDataReader"))
    {
        // void
    }

    swift::misc::aviation::CAirportList CAirportDataReader::getAirports() const
    {
        return m_airportCache.get();
    }

    CAirport CAirportDataReader::getAirportForIcaoDesignator(const QString &designator) const
    {
        return this->getAirports().findFirstByIcao(CAirportIcaoCode(designator));
    }

    CAirport CAirportDataReader::getAirportForNameOrLocation(const QString &nameOrLocation) const
    {
        return this->getAirports().findFirstByNameOrLocation(nameOrLocation);
    }

    int CAirportDataReader::getAirportsCount() const
    {
        return this->getAirports().size();
    }

    bool CAirportDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
    {
        if (dir.isEmpty() || whatToRead == CEntityFlags::NoEntity) { return false; }

        QPointer<CAirportDataReader> myself(this);
        QTimer::singleShot(0, this, [=]() {
            if (!myself) { return; }
            const CStatusMessageList msgs = this->readFromJsonFiles(dir, whatToRead, overrideNewerOnly);
            if (msgs.isFailure())
            {
                CLogMessage::preformatted(msgs);
            }
        });
        return true;
    }

    CStatusMessageList CAirportDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewerOnly)
    {
        const QDir directory(dir);
        if (!directory.exists())
        {
            return CStatusMessage(this).error(u"Missing directory '%1'") << dir;
        }

        whatToRead &= CEntityFlags::AirportEntity; // can handle these entities
        if (whatToRead == CEntityFlags::NoEntity)
        {
            return CStatusMessage(this).info(u"'%1' No entity for this reader") << CEntityFlags::entitiesToString(whatToRead);
        }

        int c = 0;
        CEntityFlags::Entity reallyRead = CEntityFlags::NoEntity;

        CStatusMessageList msgs;
        const QString fileName = CFileUtils::appendFilePaths(dir, "airports.json");
        const QFileInfo fi(fileName);
        if (!fi.exists())
        {
            msgs.push_back(CStatusMessage(this).warning(u"File '%1' does not exist") << fileName);
        }
        else if (!this->overrideCacheFromFile(overrideNewerOnly, fi, CEntityFlags::AirportEntity, msgs))
        {
            // void
        }
        else
        {
            const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
            const QJsonObject airportsJson(CDatabaseUtils::readQJsonObjectFromDatabaseFile(fileName));
            if (!airportsJson.isEmpty())
            {
                try
                {
                    const CAirportList airports = CAirportList::fromMultipleJsonFormats(airportsJson);
                    c = airports.size();
                    msgs.push_back(m_airportCache.set(airports, fi.birthTime().toUTC().toMSecsSinceEpoch()));

                    emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFinished, c, url);
                    reallyRead |= CEntityFlags::AirportEntity;
                }
                catch (const CJsonException &ex)
                {
                    emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFailed, 0, url);
                    return CStatusMessage::fromJsonException(ex, this, QStringLiteral("Reading airports from '%1'").arg(fileName));
                }
            }
        }
        return msgs;
    }

    CEntityFlags::Entity CAirportDataReader::getSupportedEntities() const
    {
        return CEntityFlags::AirportEntity;
    }

    QDateTime CAirportDataReader::getCacheTimestamp(CEntityFlags::Entity entities) const
    {
        return entities == CEntityFlags::AirportEntity ? m_airportCache.getAvailableTimestamp() : QDateTime();
    }

    int CAirportDataReader::getCacheCount(CEntityFlags::Entity entity) const
    {
        return entity == CEntityFlags::AirportEntity ? m_airportCache.get().size() : 0;
    }

    CEntityFlags::Entity CAirportDataReader::getEntitiesWithCacheCount() const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (this->getCacheCount(CEntityFlags::AirportEntity) > 0) { entities |= CEntityFlags::AirportEntity; }
        return entities;
    }

    CEntityFlags::Entity CAirportDataReader::getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (this->hasCacheTimestampNewerThan(CEntityFlags::AirportEntity, threshold)) { entities |= CEntityFlags::AirportEntity; }
        return entities;
    }

    void CAirportDataReader::synchronizeCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AirportEntity))
        {
            if (m_syncedAirportCache) { return; }
            m_syncedAirportCache = true;
            m_airportCache.synchronize();
        }
    }

    void CAirportDataReader::admitCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AirportEntity)) { m_airportCache.admit(); }
    }

    void CAirportDataReader::invalidateCaches(CEntityFlags::Entity entities)
    {
        if (entities.testFlag(CEntityFlags::AirportEntity)) { CDataCache::instance()->clearAllValues(m_airportCache.getKey()); }
    }

    bool CAirportDataReader::hasChangedUrl(CEntityFlags::Entity entity, CUrl &oldUrlInfo, CUrl &newUrlInfo) const
    {
        Q_UNUSED(entity)
        oldUrlInfo = m_readerUrlCache.get();
        newUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
        return CDatabaseReader::isChangedUrl(oldUrlInfo, newUrlInfo);
    }

    CUrl CAirportDataReader::getDbServiceBaseUrl() const
    {
        return sApp->getGlobalSetup().getDbAirportReaderUrl();
    }

    CUrl CAirportDataReader::getAirportsUrl(CDbFlags::DataRetrievalModeFlag mode) const
    {
        return this->getBaseUrl(mode).withAppendedPath(fileNameForMode(CEntityFlags::AirportEntity, mode));
    }

    void CAirportDataReader::parseAirportData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (!this->doWorkCheck()) { return; }

        const CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReplyPtr);
        const QUrl url = nwReply->url();

        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFailed, 0, url);
            return;
        }

        // parsing
        emit this->dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadParsing, 0, url);
        CAirportList airports;
        CAirportList inconsistent;
        if (res.isRestricted())
        {
            const CAirportList incrementalAirports(CAirportList::fromDatabaseJson(res, &inconsistent));
            if (incrementalAirports.isEmpty()) { return; } // currently ignored
            airports = this->getAirports();
            airports.replaceOrAddObjectsByKey(incrementalAirports);
        }
        else
        {
            QElapsedTimer time;
            time.start();
            airports = CAirportList::fromDatabaseJson(res, &inconsistent);
            this->logParseMessage("airports", airports.size(), static_cast<int>(time.elapsed()), res);
        }

        if (!inconsistent.isEmpty())
        {
            logInconsistentData(
                CStatusMessage(this, CStatusMessage::SeverityInfo, u"Inconsistent airports: " % inconsistent.dbKeysAsString(", ")),
                Q_FUNC_INFO);
        }

        if (!this->doWorkCheck()) { return; }
        const int size = airports.size();
        qint64 latestTimestamp = airports.latestTimestampMsecsSinceEpoch();
        if (size > 0 && latestTimestamp < 0)
        {
            CLogMessage(this).error(u"No timestamp in airport list, setting to last modified value");
            latestTimestamp = lastModifiedMsSinceEpoch(nwReply.data());
        }

        m_airportCache.set(airports, latestTimestamp);
        this->updateReaderUrl(getBaseUrl(CDbFlags::DbReading));

        this->emitAndLogDataRead(CEntityFlags::AirportEntity, size, res);
    }

    void CAirportDataReader::read(CEntityFlags::Entity entity, CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan)
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }
        entity &= CEntityFlags::AirportEntity;

        if (entity.testFlag(CEntityFlags::AirportEntity))
        {
            CUrl url = this->getAirportsUrl(mode);
            if (!url.isEmpty())
            {
                url.appendQuery(queryLatestTimestamp(newerThan));
                this->getFromNetworkAndLog(url, { this, &CAirportDataReader::parseAirportData });
                emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadStarted, 0, url);
            }
            else
            {
                this->logNoWorkingUrl(CEntityFlags::AirportEntity);
            }
        }
    }

    void CAirportDataReader::airportCacheChanged()
    {
        this->cacheHasChanged(CEntityFlags::AirportEntity);
    }

    void CAirportDataReader::baseUrlCacheChanged()
    {
        // void
    }

    void CAirportDataReader::updateReaderUrl(const CUrl &url)
    {
        const CUrl current = m_readerUrlCache.get();
        if (current == url) { return; }
        const CStatusMessage m = m_readerUrlCache.set(url);
        if (m.isFailure())
        {
            CLogMessage::preformatted(m);
        }
    }
} // ns
