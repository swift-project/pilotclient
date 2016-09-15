/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airportdatareader.h"
#include "blackcore/application.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Db
    {
        CAirportDataReader::CAirportDataReader(QObject* parent, const CDatabaseReaderConfigList &config) :
            CDatabaseReader(parent, config, QStringLiteral("CAirportDataReader"))
        {
            // void
        }

        BlackMisc::Aviation::CAirportList CAirportDataReader::getAirports() const
        {
            return m_airportCache.get();
        }

        QDateTime CAirportDataReader::getCacheTimestamp(CEntityFlags::Entity entities) const
        {
            return entities == CEntityFlags::AirportEntity ? m_airportCache.getAvailableTimestamp() : QDateTime();
        }

        int CAirportDataReader::getCacheCount(CEntityFlags::Entity entity) const
        {
            return entity == CEntityFlags::AirportEntity ? m_airportCache.get().size() : 0;
        }

        void CAirportDataReader::synchronizeCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::AirportEntity)) { this->m_airportCache.synchronize(); }
        }

        void CAirportDataReader::invalidateCaches(CEntityFlags::Entity entities)
        {
            if (entities.testFlag(CEntityFlags::AirportEntity)) { CDataCache::instance()->clearAllValues(this->m_airportCache.getKey()); }
        }

        bool CAirportDataReader::hasChangedUrl(CEntityFlags::Entity entity) const
        {
            Q_UNUSED(entity);
            return CDatabaseReader::isChangedUrl(this->m_readerUrlCache.get(), this->getAirportsUrl());
        }

        CUrl CAirportDataReader::getAirportsUrl() const
        {
            return sApp->getGlobalSetup().getDbRootDirectoryUrl().withAppendedPath("service/jsonairport.php");
        }

        void CAirportDataReader::ps_parseAirportData(QNetworkReply *nwReply)
        {
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply);
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            CAirportList airports;

            if (res.isRestricted())
            {
                airports = this->getAirports();
                CAirportList updates;
                updates.convertFromDatabaseJson(res);
                airports.replaceOrAddObjectsByKey(updates);
            }
            else
            {
                airports.convertFromDatabaseJson(res);
            }

            int size = airports.size();
            qint64 timestamp = lastModifiedMsSinceEpoch(nwReply);
            if (size > 0 && timestamp < 0)
            {
                CLogMessage(this).error("No timestamp in airport list, setting to last modified value");
                timestamp = lastModifiedMsSinceEpoch(nwReply);
            }

            m_airportCache.set(airports, timestamp);

            emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFinished, airports.size());
        }

        void CAirportDataReader::ps_read(CEntityFlags::Entity entity, const QDateTime &newerThan)
        {
            this->threadAssertCheck();
            if (this->isAbandoned()) { return; }

            if (entity.testFlag(CEntityFlags::AirportEntity))
            {
                CUrl url = getAirportsUrl();
                if (!url.isEmpty())
                {
                    if (!newerThan.isNull())
                    {
                        const QString tss(newerThan.toString(Qt::ISODate));
                        url.appendQuery(QString(parameterLatestTimestamp() + "=" + tss));
                    }
                    sApp->getFromNetwork(url, { this, &CAirportDataReader::ps_parseAirportData });
                    emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::StartRead, 0);
                }
                else
                {
                    CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::AirportEntity);
                }
            }
        }

        void CAirportDataReader::ps_airportCacheChanged()
        {
            // void
        }

        void CAirportDataReader::ps_baseUrlCacheChanged()
        {
            // void
        }
    } // ns
} // ns
