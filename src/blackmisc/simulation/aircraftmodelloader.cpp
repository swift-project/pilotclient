/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/compare.h"
#include "blackmisc/logmessage.h"

#include <QDir>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackMisc
{
    namespace Simulation
    {
        IAircraftModelLoader::IAircraftModelLoader(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
            this->m_caches.setCurrentSimulator(simulator);

            // first connect is an internal connection to log info about load status
            connect(this, &IAircraftModelLoader::loadingFinished, this, &IAircraftModelLoader::ps_loadFinished);
            connect(&this->m_caches, &IMultiSimulatorModelCaches::cacheChanged, this, &IAircraftModelLoader::ps_cacheChanged);
        }

        IAircraftModelLoader::~IAircraftModelLoader()
        {
            this->gracefulShutdown();
        }

        const CLogCategoryList &IAircraftModelLoader::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::modelLoader() });
            return cats;
        }

        CStatusMessage IAircraftModelLoader::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->getSimulator(); // support default value
            return this->m_caches.setCachedModels(models, sim);
        }

        CStatusMessage IAircraftModelLoader::replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->getSimulator(); // support default values
            CAircraftModelList allModels(this->m_caches.getSynchronizedCachedModels(sim));
            const int c = allModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (c > 0)
            {
                return this->setCachedModels(allModels, sim);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data changed");
            }
        }

        void IAircraftModelLoader::ps_loadFinished(const CStatusMessage &status, const BlackMisc::Simulation::CSimulatorInfo &simulator, LoadFinishedInfo info)
        {
            Q_UNUSED(info);

            // remark: in the past status used to be bool, now it is CStatusMessage
            // so there is some redundancy here between status and m_loadingMessages
            this->m_loadingInProgress = false;

            if (this->m_loadingMessages.hasWarningOrErrorMessages())
            {
                CLogMessage::preformatted(this->m_loadingMessages);
            }
            else
            {
                CLogMessage(this).info("Loading finished, success '%1' for '%2'") << status.getMessage() << simulator.toQString();
            }
        }

        void IAircraftModelLoader::ps_cacheChanged(const CSimulatorInfo &simInfo)
        {
            static const CStatusMessage status(this, CStatusMessage::SeverityInfo, "Cached changed");
            emit this->loadingFinished(status, simInfo, CacheLoaded);
        }

        QStringList IAircraftModelLoader::getModelDirectoriesOrDefault() const
        {
            const QStringList mdirs = this->m_settings.getModelDirectoriesOrDefault(this->getSimulator());
            return mdirs;
        }

        QString IAircraftModelLoader::getFirstModelDirectoryOrDefault() const
        {
            const QString md = this->m_settings.getFirstModelDirectoryOrDefault(this->getSimulator());
            return md;
        }

        QStringList IAircraftModelLoader::getModelExcludeDirectoryPatterns() const
        {
            return this->m_settings.getModelExcludeDirectoryPatternsOrDefault(this->getSimulator());
        }

        CAircraftModelList IAircraftModelLoader::getAircraftModels() const
        {
            return this->m_caches.getCurrentCachedModels();
        }

        CAircraftModelList IAircraftModelLoader::getCachedAircraftModels(const CSimulatorInfo &simulator) const
        {
            return this->m_caches.getCachedModels(simulator);
        }

        QDateTime IAircraftModelLoader::getCacheTimestamp() const
        {
            return this->m_caches.getCurrentCacheTimestamp();
        }

        bool IAircraftModelLoader::hasCachedData() const
        {
            return !this->m_caches.getCurrentCachedModels().isEmpty();
        }

        CStatusMessage IAircraftModelLoader::clearCache()
        {
            return this->setCachedModels(CAircraftModelList());
        }

        void IAircraftModelLoader::startLoading(LoadMode mode, const ModelConsolidation &modelConsolidation, const QString &directory)
        {
            if (this->m_loadingInProgress) { return; }
            this->m_loadingInProgress = true;
            const bool useCachedData = !mode.testFlag(CacheSkipped) && this->hasCachedData();
            if (useCachedData && (mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly)))
            {
                // we just jus cache data
                static const CStatusMessage status(this, CStatusMessage::SeverityInfo, "Using cached data");
                emit loadingFinished(status, this->getSimulator(), CacheLoaded);
                return;
            }
            if (mode.testFlag(CacheOnly))
            {
                // only cache, but we did not find any data yet (still in progress?)
                // here we rely on the cache load slot, no need to emit here, will
                // be done later in ps_cacheChanged. An alternative was to sync cache here
                this->m_loadingInProgress = false;
                return;
            }

            // really load from disk
            this->startLoadingFromDisk(mode, modelConsolidation, directory);
        }

        const CSimulatorInfo IAircraftModelLoader::getSimulator() const
        {
            return this->m_caches.getCurrentSimulator();
        }

        QString IAircraftModelLoader::getSimulatorAsString() const
        {
            return this->getSimulator().toQString();
        }

        bool IAircraftModelLoader::supportsSimulator(const CSimulatorInfo &simulator)
        {
            return getSimulator().matchesAny(simulator);
        }

        void IAircraftModelLoader::cancelLoading()
        {
            this->m_cancelLoading = true;
            this->m_loadingInProgress = true;
        }

        void IAircraftModelLoader::gracefulShutdown()
        {
            this->cancelLoading();
        }

        QString IAircraftModelLoader::getInfoString() const
        {
            return this->m_caches.getInfoString();
        }

        QString IAircraftModelLoader::getInfoStringFsFamily() const
        {
            return this->m_caches.getInfoStringFsFamily();
        }

        std::unique_ptr<IAircraftModelLoader> IAircraftModelLoader::createModelLoader(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Single simulator");
            std::unique_ptr<IAircraftModelLoader> loader;
            if (simulator.xplane())
            {
                loader = std::make_unique<CAircraftModelLoaderXPlane>();
            }
            else
            {
                loader = CAircraftCfgParser::createModelLoader(simulator);
            }

            if (!loader) { return loader; }

            // make sure the cache is really available, normally this happens in the constructor
            if (loader->getSimulator() != simulator)
            {
                loader->m_caches.setCurrentSimulator(simulator); // mark current simulator and sync caches
            }
            return loader;
        }
    } // ns
} // ns
