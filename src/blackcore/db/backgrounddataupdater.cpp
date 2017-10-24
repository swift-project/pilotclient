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
                connect(sApp->getWebDataServices()->getDatabaseWriter(), &CDatabaseWriter::publishedModelsSimplified, this, &CBackgroundDataUpdater::onModelsPublished);
            }
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
                this->triggerInfoReads();
                break;
            case 1:
                this->syncDbEntity(CEntityFlags::ModelEntity);
                this->syncDbEntity(CEntityFlags::DistributorEntity);
                break;
            case 2:
                this->syncModelOrModelSetCacheWithDbData(m_modelCaches);
                break;
            case 3:
                this->syncModelOrModelSetCacheWithDbData(m_modelSetCaches);
                break;
            case 4:
                this->syncDbEntity(CEntityFlags::AircraftIcaoEntity);
                this->syncDbEntity(CEntityFlags::AirlineIcaoEntity);
                break;
            default:
                break;
            }
            ++m_cycle %= 5;
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

            const QDateTime dbModelsLatestSync = m_syncedModelsLatestChange.value(cache.getDescription());
            if (dbModelsLatestSync.isValid() && latestDbModelsTs <= dbModelsLatestSync) { return; }

            m_syncedModelsLatestChange[cache.getDescription()] = latestDbModelsTs;
            const CSimulatorInfo sims = cache.simulatorsWithInitializedCache(); // sims ever used
            if (sims.isNoSimulator()) { return; }

            const CAircraftModelList dbModels = dbModelsConsidered.isEmpty() ?
                                                sApp->getWebDataServices()->getModels() :
                                                dbModelsConsidered;
            if (dbModels.isEmpty()) { return; }
            const QSet<CSimulatorInfo> simSet = sims.asSingleSimulatorSet();
            for (const CSimulatorInfo &singleInfo : simSet)
            {
                if (!this->doWorkCheck()) { return; }
                CAircraftModelList simModels = cache.getSynchronizedCachedModels(singleInfo);
                if (simModels.isEmpty()) { continue; }
                const CAircraftModelList dbModelsForSim = dbModels.matchesSimulator(singleInfo);
                if (dbModelsForSim.isEmpty()) { continue; }

                // time consuming part
                const int c = CDatabaseUtils::consolidateModelsWithDbData(dbModelsForSim, simModels, true);
                if (c > 0)
                {
                    CLogMessage(this).info("Consolidated %1 models for '%2'") << c << singleInfo.convertToQString();
                    const CStatusMessage m = cache.setCachedModels(simModels, singleInfo);
                    CLogMessage::preformatted(m);
                }
                else
                {
                    CLogMessage(this).info("Synchronized, no changes for '%1'") << singleInfo.convertToQString();
                }
                if (simSet.size() > 1) { CEventLoop::processEventsFor(5000); } // just give the system some time to relax, consolidation is time consuming
            }
        }

        void CBackgroundDataUpdater::syncDbEntity(CEntityFlags::Entity entity) const
        {
            if (!this->doWorkCheck()) { return; }
            const QDateTime latestCacheTs = sApp->getWebDataServices()->getCacheTimestamp(entity);
            if (!latestCacheTs.isValid()) { return; }
            const QDateTime latestDbTs = sApp->getWebDataServices()->getLatestDbEntityTimestamp(entity);
            if (!latestDbTs.isValid()) { return; }
            if (latestDbTs <= latestCacheTs)
            {
                CLogMessage(this).info("No auto sync with DB, entity '%1', DB ts: %2 cache ts: %3") << CEntityFlags::flagToString(entity) << latestDbTs.toString(Qt::ISODate) << latestCacheTs.toString(Qt::ISODate);
                return;
            }

            CLogMessage(this).info("Triggering read of '%1' since '%2'") << CEntityFlags::flagToString(entity) << latestCacheTs.toString(Qt::ISODate);
            sApp->getWebDataServices()->triggerLoadingDirectlyFromDb(CEntityFlags::ModelEntity, latestCacheTs);
        }

        bool CBackgroundDataUpdater::doWorkCheck() const
        {
            if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return false; }
            if (!isEnabled()) { return false; }
            return true;
        }

        void CBackgroundDataUpdater::onModelsPublished(const CAircraftModelList &modelsPublished)
        {
            if (!this->doWorkCheck()) { return; }
            if (modelsPublished.isEmpty()) { return; }
            if (!m_updatePublishedModels)  { return; }

            emit this->consolidating(true);
            this->syncModelOrModelSetCacheWithDbData(m_modelCaches, modelsPublished);
            this->syncModelOrModelSetCacheWithDbData(m_modelSetCaches, modelsPublished);
            emit this->consolidating(false);
        }
    } // ns
} // ns
