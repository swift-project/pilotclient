/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/compare.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"

#include <QDir>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Simulation::Settings;
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
            connect(this, &IAircraftModelLoader::loadingFinished, this, &IAircraftModelLoader::ps_loadFinished);
        }

        IAircraftModelLoader::~IAircraftModelLoader()
        {
            this->gracefulShutdown();
        }

        bool IAircraftModelLoader::existsDir(const QString &directory) const
        {
            if (directory.isEmpty()) { return false; }
            QDir dir(directory);

            //! \todo not available network dir can make this hang here, however there is no obvious solution to that
            return dir.exists();
        }

        CStatusMessage IAircraftModelLoader::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->getSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simuataor"); }
            return this->m_caches.setCachedModels(models, sim);
        }

        CStatusMessage IAircraftModelLoader::replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->getSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simuataor"); }
            CAircraftModelList allModels(this->m_caches.getSyncronizedCachedModels(sim));
            int c = allModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (c > 0)
            {
                return this->setCachedModels(allModels, sim);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data changed");
            }
        }

        void IAircraftModelLoader::ps_loadFinished(bool success)
        {
            Q_UNUSED(success);
            this->m_loadingInProgress = false;
        }

        QString IAircraftModelLoader::getModelDirectory() const
        {
            return this->m_settings.getModelDirectoryOrDefault(this->getSimulator());
        }

        QStringList IAircraftModelLoader::getModelExcludeDirectories(bool relative) const
        {
            return this->m_settings.getModelExcludeDirectoryPatternsOrDefault(this->getSimulator(), relative);
        }

        CAircraftModelList IAircraftModelLoader::getAircraftModels() const
        {
            return this->m_caches.getCurrentCachedModels();
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

        void IAircraftModelLoader::startLoading(LoadMode mode, const CAircraftModelList &dbModels)
        {
            if (this->m_loadingInProgress) { return; }
            this->m_loadingInProgress = true;
            const bool useCachedData = !mode.testFlag(CacheSkipped) && this->hasCachedData();
            if (useCachedData && (mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly)))
            {
                emit loadingFinished(true, this->getSimulator());
                return;
            }
            else if (useCachedData && mode.testFlag(CacheUntilNewer))
            {
                //! \todo currently too slow, does not make sense with that overhead
                if (!this->areModelFilesUpdated())
                {
                    emit loadingFinished(true, this->getSimulator());
                    return;
                }
            }
            if (mode.testFlag(CacheOnly))
            {
                // only cache, but we did not find any data
                emit loadingFinished(false, this->getSimulator());
                return;
            }
            this->startLoadingFromDisk(mode, dbModels);
        }

        const CSimulatorInfo IAircraftModelLoader::getSimulator() const
        {
            return this->m_caches.getCurrentSimulator();
        }

        QString IAircraftModelLoader::getSimulatorAsString() const
        {
            return this->getSimulator().toQString();
        }

        bool IAircraftModelLoader::supportsSimulator(const CSimulatorInfo &info)
        {
            return getSimulator().matchesAny(info);
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
