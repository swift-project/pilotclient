/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "backgrounddataupdater.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/db/databasewriter.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/eventloop.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCore
{
    namespace Db
    {
        const CLogCategoryList &CBackgroundDataUpdater::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::worker() };
            return cats;
        }

        CBackgroundDataUpdater::CBackgroundDataUpdater(QObject *owner) :
            CContinuousWorker(owner, "Background data updater")
        {
            connect(&m_updateTimer, &QTimer::timeout, this, &CBackgroundDataUpdater::doWork);
            m_updateTimer.setInterval(60 * 1000);
            if (sApp && sApp->hasWebDataServices())
            {
                connect(sApp->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::publishedModelsSimplified, this, &CBackgroundDataUpdater::onModelsPublished, Qt::QueuedConnection);
            }
        }

        CStatusMessageList CBackgroundDataUpdater::getMessageHistory() const
        {
            QReadLocker l(&m_lockMsg);
            return m_messageHistory;
        }

        void CBackgroundDataUpdater::doWork()
        {
            if (!this->doWorkCheck()) { return; }
            m_inWork = true;

            emit this->consolidating(true);
            const int cycle = m_cycle;
            switch (cycle)
            {
            case 0:
                // normally redundant, will be read in other places as well
                // new metadata for next comparison
                this->addHistory(CLogMessage(this).info("Triggered info reads from DB"));
                this->triggerInfoReads();
                break;
            case 1:
                this->addHistory(CLogMessage(this).info("Synchronize DB entities"));
                this->syncDbEntity(CEntityFlags::AirlineIcaoEntity);
                this->syncDbEntity(CEntityFlags::LiveryEntity);
                this->syncDbEntity(CEntityFlags::ModelEntity);
                this->syncDbEntity(CEntityFlags::DistributorEntity);
                this->syncDbEntity(CEntityFlags::AircraftIcaoEntity);
                break;
            case 2:
                this->addHistory(CLogMessage(this).info("Synchronize %1") << m_modelCaches.getDescription());
                this->syncModelOrModelSetCacheWithDbData(m_modelCaches);
                break;
            case 3:
                this->addHistory(CLogMessage(this).info("Synchronize %1") << m_modelSetCaches.getDescription());
                this->syncModelOrModelSetCacheWithDbData(m_modelSetCaches);
                break;
            default:
                break;
            }
            ++m_cycle %= 4;

            m_inWork = false;
            emit this->consolidating(false);
        }

        void CBackgroundDataUpdater::triggerInfoReads()
        {
            if (!this->doWorkCheck()) { return; }
            sApp->getWebDataServices()->triggerReadOfDbInfoObjects();
            sApp->getWebDataServices()->triggerReadOfSharedInfoObjects();
        }

        void CBackgroundDataUpdater::syncModelOrModelSetCacheWithDbData(IMultiSimulatorModelCaches &cache, const CAircraftModelList &dbModelsConsidered)
        {
            if (!this->doWorkCheck()) { return; }
            const QDateTime latestDbModelsTs = dbModelsConsidered.isEmpty() ?
                                               sApp->getWebDataServices()->getCacheTimestamp(CEntityFlags::ModelEntity) :
                                               dbModelsConsidered.latestTimestamp();
            if (!latestDbModelsTs.isValid()) { return; }

            // newer DB models as cache
            const QDateTime dbModelsLatestSync = m_syncedModelsLatestChange.value(cache.getDescription());
            if (dbModelsLatestSync.isValid() && latestDbModelsTs <= dbModelsLatestSync) { return; }

            m_syncedModelsLatestChange[cache.getDescription()] = latestDbModelsTs;
            const CSimulatorInfo simulators = cache.simulatorsWithInitializedCache(); // simulators ever used
            if (simulators.isNoSimulator()) { return; }

            const CAircraftModelList dbModels = dbModelsConsidered.isEmpty() ?
                                                sApp->getWebDataServices()->getModels() :
                                                dbModelsConsidered;
            if (dbModels.isEmpty()) { return; }
            const QSet<CSimulatorInfo> simulatorsSet = simulators.asSingleSimulatorSet();
            for (const CSimulatorInfo &singleSimulator : simulatorsSet)
            {
                if (!this->doWorkCheck()) { return; }
                CAircraftModelList simulatorModels = cache.getSynchronizedCachedModels(singleSimulator);
                if (simulatorModels.isEmpty()) { continue; }
                const CAircraftModelList dbModelsForSimulator = dbModels.matchesSimulator(singleSimulator);
                if (dbModelsForSimulator.isEmpty()) { continue; }

                // time consuming part
                const int c = CDatabaseUtils::consolidateModelsWithDbData(dbModelsForSimulator, simulatorModels, true);
                if (c > 0)
                {
                    this->addHistory(CLogMessage(this).info("Consolidated %1 models for '%2'") << c << singleSimulator.convertToQString());
                    const CStatusMessage m = cache.setCachedModels(simulatorModels, singleSimulator);
                    CLogMessage::preformatted(m);
                    this->addHistory(m);
                }
                else
                {
                    this->addHistory(CLogMessage(this).info("Synchronized, no changes for '%1'") << singleSimulator.convertToQString());
                }

                if (simulatorsSet.size() > 1)
                {
                    // just give the system some time to relax, consolidation is time consuming
                    if (!this->doWorkCheck()) { return; }
                    CEventLoop::processEventsFor(1000);
                }
            }
        }

        void CBackgroundDataUpdater::syncDbEntity(CEntityFlags::Entity entity)
        {
            if (!this->doWorkCheck()) { return; }
            const QDateTime latestCacheTs = sApp->getWebDataServices()->getCacheTimestamp(entity);
            if (!latestCacheTs.isValid()) { return; }

            const QDateTime latestDbTs = sApp->getWebDataServices()->getLatestDbEntityTimestamp(entity);
            const QString entityStr = CEntityFlags::flagToString(entity);
            const QString latestCacheTsStr = latestCacheTs.toString(Qt::ISODate);

            if (!latestDbTs.isValid()) { return; }
            if (latestDbTs <= latestCacheTs)
            {
                this->addHistory(CLogMessage(this).info("No auto sync with DB, entity '%1', DB ts: %2 cache ts: %3") << entityStr << latestDbTs.toString(Qt::ISODate) << latestCacheTsStr);
                return;
            }

            this->addHistory(CLogMessage(this).info("Triggering read of '%1' since '%2'") << entityStr << latestCacheTsStr);
            sApp->getWebDataServices()->triggerLoadingDirectlyFromDb(entity, latestCacheTs);
        }

        bool CBackgroundDataUpdater::doWorkCheck() const
        {
            if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return false; }
            if (!this->isEnabled()) { return false; }
            return true;
        }

        void CBackgroundDataUpdater::onModelsPublished(const CAircraftModelList &modelsPublished, bool directWrite)
        {
            if (!this->doWorkCheck()) { return; }
            if (modelsPublished.isEmpty()) { return; }
            if (!m_updatePublishedModels)  { return; }
            if (!directWrite) { return; } // those models are CRs and have to be released first

            emit this->consolidating(true);
            this->syncModelOrModelSetCacheWithDbData(m_modelCaches, modelsPublished);
            this->syncModelOrModelSetCacheWithDbData(m_modelSetCaches, modelsPublished);
            emit this->consolidating(false);
        }

        void CBackgroundDataUpdater::addHistory(const CStatusMessage &msg)
        {
            QWriteLocker l(&m_lockMsg);
            m_messageHistory.push_frontMaxElements(msg, 100); // latest
        }
    } // ns
} // ns
