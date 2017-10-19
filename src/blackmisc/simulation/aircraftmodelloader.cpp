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
#include "blackmisc/directoryutils.h"
#include "blackmisc/compare.h"
#include "blackmisc/logmessage.h"

#include <QDir>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
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
            m_caches.setCurrentSimulator(simulator);

            // first connect is an internal connection to log info about load status
            connect(this, &IAircraftModelLoader::loadingFinished, this, &IAircraftModelLoader::loadFinished);
            connect(&m_caches, &IMultiSimulatorModelCaches::cacheChanged, this, &IAircraftModelLoader::cacheChanged);
        }

        QString IAircraftModelLoader::enumToString(IAircraftModelLoader::LoadFinishedInfo info)
        {
            switch (info)
            {
            case CacheLoaded: return "cache loaded";
            case LoadingSkipped: return "loading skipped";
            case ParsedData: return "parsed data";
            default: break;
            }
            return "??";
        }

        QString IAircraftModelLoader::enumToString(IAircraftModelLoader::LoadModeFlag modeFlag)
        {
            switch (modeFlag)
            {
            case NotSet: return "not set";
            case LoadDirectly: return "load directly";
            case LoadInBackground: return "load in background";
            case CacheFirst: return "cache first";
            case CacheSkipped: return "cache skipped";
            case CacheOnly: return "cache only";
            default: break;
            }
            return "??";
        }

        QString IAircraftModelLoader::enumToString(LoadMode mode)
        {
            QStringList modes;
            if (mode.testFlag(NotSet)) modes << enumToString(NotSet);
            if (mode.testFlag(LoadDirectly)) modes << enumToString(LoadDirectly);
            if (mode.testFlag(LoadInBackground)) modes << enumToString(LoadInBackground);
            if (mode.testFlag(CacheFirst)) modes << enumToString(CacheFirst);
            if (mode.testFlag(CacheSkipped)) modes << enumToString(CacheSkipped);
            return modes.join(", ");
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
            return m_caches.setCachedModels(models, sim);
        }

        CStatusMessage IAircraftModelLoader::replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->getSimulator(); // support default values
            CAircraftModelList allModels(m_caches.getSynchronizedCachedModels(sim));
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

        void IAircraftModelLoader::loadFinished(const CStatusMessage &status, const BlackMisc::Simulation::CSimulatorInfo &simulator, LoadFinishedInfo info)
        {
            Q_UNUSED(info);

            // remark: in the past status used to be bool, now it is CStatusMessage
            // so there is some redundancy here between status and m_loadingMessages
            m_loadingInProgress = false;

            if (m_loadingMessages.hasWarningOrErrorMessages())
            {
                CLogMessage::preformatted(m_loadingMessages);
            }
            else
            {
                CLogMessage(this).info("Loading finished, success '%1' for '%2'") << status.getMessage() << simulator.toQString();
            }
        }

        void IAircraftModelLoader::cacheChanged(const CSimulatorInfo &simInfo)
        {
            static const CStatusMessage status(this, CStatusMessage::SeverityInfo, "Cached changed");
            emit this->loadingFinished(status, simInfo, CacheLoaded);
        }

        QStringList IAircraftModelLoader::getModelDirectoriesOrDefault() const
        {
            const QStringList mdirs = m_settings.getModelDirectoriesOrDefault(this->getSimulator());
            return mdirs;
        }

        QString IAircraftModelLoader::getFirstModelDirectoryOrDefault() const
        {
            const QString md = m_settings.getFirstModelDirectoryOrDefault(this->getSimulator());
            return md;
        }

        QStringList IAircraftModelLoader::getModelExcludeDirectoryPatterns() const
        {
            return m_settings.getModelExcludeDirectoryPatternsOrDefault(this->getSimulator());
        }

        CAircraftModelList IAircraftModelLoader::getAircraftModels() const
        {
            return m_caches.getCurrentCachedModels();
        }

        CAircraftModelList IAircraftModelLoader::getCachedAircraftModels(const CSimulatorInfo &simulator) const
        {
            return m_caches.getCachedModels(simulator);
        }

        QDateTime IAircraftModelLoader::getCacheTimestamp() const
        {
            return m_caches.getCurrentCacheTimestamp();
        }

        bool IAircraftModelLoader::hasCachedData() const
        {
            return !m_caches.getCurrentCachedModels().isEmpty();
        }

        CStatusMessage IAircraftModelLoader::clearCache()
        {
            return this->setCachedModels(CAircraftModelList());
        }

        void IAircraftModelLoader::startLoading(LoadMode mode, const ModelConsolidation &modelConsolidation, const QString &directory)
        {
            if (m_loadingInProgress) { return; }
            m_loadingInProgress = true;
            m_loadingMessages.clear();

            const bool useCachedData = !mode.testFlag(CacheSkipped) && this->hasCachedData();
            if (useCachedData && (mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly)))
            {
                // we just just cache data
                static const CStatusMessage status(this, CStatusMessage::SeverityInfo, "Using cached data");
                emit loadingFinished(status, this->getSimulator(), CacheLoaded);
                return;
            }
            if (mode.testFlag(CacheOnly))
            {
                // only cache, but we did not find any data yet (still in progress?)
                // here we rely on the cache load slot, no need to emit here, will
                // be done later in ps_cacheChanged. An alternative was to sync cache here
                m_loadingInProgress = false;
                return;
            }

            // really load from disk?
            if (m_skipLoadingEmptyModelDir && !CDirectoryUtils::existsUnemptyDirectory(directory))
            {
                const CStatusMessage status = CStatusMessage(this, CStatusMessage::SeverityWarning, "Empty or not existing %1 directory '%2', skipping read")
                                              << this->getSimulator().toQString() << directory;
                m_loadingMessages.push_back(status);
                emit loadingFinished(status, this->getSimulator(), LoadingSkipped);
                return;
            }

            this->startLoadingFromDisk(mode, modelConsolidation, directory);
        }

        const CSimulatorInfo IAircraftModelLoader::getSimulator() const
        {
            return m_caches.getCurrentSimulator();
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
            if (!m_loadingInProgress) { return; }
            m_cancelLoading = true;
        }

        void IAircraftModelLoader::gracefulShutdown()
        {
            this->cancelLoading();
            m_loadingInProgress = true; // avoids further startups
        }

        QString IAircraftModelLoader::getInfoString() const
        {
            return m_caches.getInfoString();
        }

        QString IAircraftModelLoader::getInfoStringFsFamily() const
        {
            return m_caches.getInfoStringFsFamily();
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
