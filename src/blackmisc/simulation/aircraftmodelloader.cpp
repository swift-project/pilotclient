/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"

using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackMisc
{
    namespace Simulation
    {
        IAircraftModelLoader::IAircraftModelLoader(const CSimulatorInfo &info, const QString &rootDirectory, const QStringList &excludeDirs) :
            m_simulatorInfo(info), m_rootDirectory(rootDirectory), m_excludedDirectories(excludeDirs)
        {
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
            //! \todo not available network dir can make this hang here
            return dir.exists();
        }

        void IAircraftModelLoader::ps_loadFinished(bool success)
        {
            Q_UNUSED(success);
            this->m_loadingInProgress = false;
        }

        bool IAircraftModelLoader::changeRootDirectory(const QString &directory)
        {
            if (m_rootDirectory == directory) { return false; }
            if (directory.isEmpty() || !existsDir(directory)) { return false; }

            m_rootDirectory = directory;
            return true;
        }

        void IAircraftModelLoader::startLoading(LoadMode mode)
        {
            if (this->m_loadingInProgress) { return; }
            this->m_loadingInProgress = true;
            const bool useCachedData = !mode.testFlag(CacheSkipped) && this->hasCachedData();
            if (useCachedData && (mode.testFlag(CacheFirst) || mode.testFlag(CacheOnly)))
            {
                emit loadingFinished(true, this->m_simulatorInfo);
                return;
            }
            else if (useCachedData && mode.testFlag(CacheUntilNewer))
            {
                if (!this->areModelFilesUpdated())
                {
                    emit loadingFinished(true, this->m_simulatorInfo);
                    return;
                }
            }
            if (mode.testFlag(CacheOnly))
            {
                // only cache, but we did not find any data
                emit loadingFinished(false, this->m_simulatorInfo);
                return;
            }
            this->startLoadingFromDisk(mode);
        }

        const CSimulatorInfo &IAircraftModelLoader::supportedSimulators() const
        {
            return m_simulatorInfo;
        }

        QString IAircraftModelLoader::supportedSimulatorsAsString() const
        {
            return m_simulatorInfo.toQString();
        }

        bool IAircraftModelLoader::supportsSimulator(const CSimulatorInfo &info)
        {
            return supportedSimulators().matchesAny(info);
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

        std::unique_ptr<IAircraftModelLoader> IAircraftModelLoader::createModelLoader(const CSimulatorInfo &simInfo)
        {
            if (simInfo.xplane())
            {
                return std::make_unique<CAircraftModelLoaderXPlane>(
                           CSimulatorInfo(CSimulatorInfo::XPLANE),
                           CXPlaneUtil::xplaneRootDir());
            }
            else
            {
                return CAircraftCfgParser::createModelLoader(simInfo);
            }
        }
    } // ns
} // ns
