// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/db/backgrounddataupdater.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/db/databasewriter.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/eventloop.h"
#include "blackmisc/logmessage.h"
#include <QElapsedTimer>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCore::Db
{
    const QStringList &CBackgroundDataUpdater::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::worker(), CLogCategories::modelSetCache(), CLogCategories::modelCache() });
        return cats;
    }

    CBackgroundDataUpdater::CBackgroundDataUpdater(QObject *owner) : CContinuousWorker(owner, "Background data updater")
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
        if (m_inWork) { return; }
        m_inWork = true;

        emit this->consolidating(true);
        int cycle = m_cycle;
        this->addHistory(CLogMessage(this).info(u"Background consolidation cycle %1") << cycle);

        switch (cycle)
        {
        case 0:
            // normally redundant, will be read in other places as well
            // new metadata for next comparison
            this->addHistory(CLogMessage(this).info(u"Triggered info reads from DB"));
            this->triggerInfoReads();
            break;
        case 1:
            this->addHistory(CLogMessage(this).info(u"Synchronize DB entities"));
            this->syncDbEntity(CEntityFlags::AirlineIcaoEntity);
            this->syncDbEntity(CEntityFlags::LiveryEntity);
            this->syncDbEntity(CEntityFlags::ModelEntity);
            this->syncDbEntity(CEntityFlags::DistributorEntity);
            this->syncDbEntity(CEntityFlags::AircraftIcaoEntity);
            break;
        case 2:
            this->addHistory(CLogMessage(this).info(u"Synchronize %1") << this->modelCaches(true).getDescription());
            this->syncModelOrModelSetCacheWithDbData(true); // set
            break;
        case 3:
            this->addHistory(CLogMessage(this).info(u"Synchronize %1") << this->modelCaches(false).getDescription());
            this->syncModelOrModelSetCacheWithDbData(false);
            break;
        default:
            break;
        }

        ++cycle %= 4;
        m_cycle = cycle;
        m_inWork = false;
        emit this->consolidating(false);
    }

    void CBackgroundDataUpdater::triggerInfoReads()
    {
        if (!this->doWorkCheck()) { return; }
        sApp->getWebDataServices()->triggerReadOfDbInfoObjects();
        sApp->getWebDataServices()->triggerReadOfSharedInfoObjects();
    }

    void CBackgroundDataUpdater::syncModelOrModelSetCacheWithDbData(bool modelSetFlag, const CAircraftModelList &dbModelsConsidered)
    {
        if (!this->doWorkCheck()) { return; }
        IMultiSimulatorModelCaches &modelCaches = this->modelCaches(modelSetFlag);
        const QDateTime latestDbModelsTs = dbModelsConsidered.isEmpty() ?
                                               sApp->getWebDataServices()->getCacheTimestamp(CEntityFlags::ModelEntity) :
                                               dbModelsConsidered.latestTimestamp();
        if (!latestDbModelsTs.isValid()) { return; }

        // newer DB models as cache
        const QDateTime dbModelsLatestSync = m_syncedModelsLatestChange.value(modelCaches.getDescription());
        if (dbModelsLatestSync.isValid() && latestDbModelsTs <= dbModelsLatestSync) { return; }

        const QString description = modelCaches.getDescription();
        m_syncedModelsLatestChange[description] = latestDbModelsTs;
        const CSimulatorInfo simulators = modelCaches.simulatorsWithInitializedCache(); // simulators ever used
        if (simulators.isNoSimulator()) { return; }

        const CAircraftModelList dbModels = dbModelsConsidered.isEmpty() ?
                                                sApp->getWebDataServices()->getModels() :
                                                dbModelsConsidered;
        if (dbModels.isEmpty()) { return; }
        const QSet<CSimulatorInfo> simulatorsSet = simulators.asSingleSimulatorSet();
        QElapsedTimer time;
        for (const CSimulatorInfo &singleSimulator : simulatorsSet)
        {
            if (!this->doWorkCheck()) { return; }
            CAircraftModelList simulatorModels = modelCaches.getSynchronizedCachedModels(singleSimulator);
            if (simulatorModels.isEmpty()) { continue; }
            time.restart();
            const CAircraftModelList dbModelsForSimulator = dbModels.matchesSimulator(singleSimulator);
            if (dbModelsForSimulator.isEmpty()) { continue; }

            // time consuming part
            const int c = CDatabaseUtils::consolidateModelsWithDbData(dbModelsForSimulator, simulatorModels, true);
            if (c > 0)
            {
                const CStatusMessage m = modelCaches.setCachedModels(simulatorModels, singleSimulator);
                const int msElapsed = time.elapsed();
                this->addHistory(CLogMessage(this).info(u"Consolidated %1 models (%2) for '%3' in %4ms") << c << description << singleSimulator.convertToQString() << msElapsed);
                CLogMessage::preformatted(m);
                this->addHistory(m);
            }
            else
            {
                this->addHistory(CLogMessage(this).info(u"Synchronized, no changes for '%1'") << singleSimulator.convertToQString());
            }

            if (simulatorsSet.size() > 1)
            {
                // just give the system some time to relax, consolidation is time consuming
                if (!this->doWorkCheck()) { return; }
                CEventLoop eventLoop(this);
                eventLoop.exec(1000);
            }
        }
    }

    void CBackgroundDataUpdater::syncDbEntity(CEntityFlags::Entity entity)
    {
        if (!this->doWorkCheck()) { return; }
        const QDateTime latestCacheTs = sApp->getWebDataServices()->getCacheTimestamp(entity);
        if (!latestCacheTs.isValid()) { return; }

        const QDateTime latestDbTs = sApp->getWebDataServices()->getLatestDbEntityTimestamp(entity);
        const QString entityStr = CEntityFlags::entitiesToString(entity);
        const QString latestCacheTsStr = latestCacheTs.toString(Qt::ISODate);

        if (!latestDbTs.isValid()) { return; }
        if (latestDbTs <= latestCacheTs)
        {
            this->addHistory(CLogMessage(this).info(u"Background updater (%1), no auto synchronization with DB, entity '%2', DB ts: %3 cache ts: %4")
                             << CThreadUtils::currentThreadInfo() << entityStr << latestDbTs.toString(Qt::ISODate) << latestCacheTsStr);
            return;
        }

        this->addHistory(CLogMessage(this).info(u"Background updater (%1) triggering read of '%2' since '%3'")
                         << CThreadUtils::currentThreadInfo() << entityStr << latestCacheTsStr);
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
        if (!m_updatePublishedModels) { return; }
        if (!directWrite) { return; } // those models are CRs and have to be released first
        if (m_inWork) { return; } // no 2 updates at the same time

        emit this->consolidating(true);
        this->syncModelOrModelSetCacheWithDbData(true, modelsPublished);
        this->syncModelOrModelSetCacheWithDbData(false, modelsPublished);
        emit this->consolidating(false);
    }

    IMultiSimulatorModelCaches &CBackgroundDataUpdater::modelCaches(bool modelSetFlag)
    {
        if (modelSetFlag) { return m_modelSets; }
        return m_modelCaches;
    }

    void CBackgroundDataUpdater::addHistory(const CStatusMessage &msg)
    {
        QWriteLocker l(&m_lockMsg);
        m_messageHistory.push_frontMaxElements(msg, 100); // latest
    }
} // ns
