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
        IAircraftModelLoader::IAircraftModelLoader(const CSimulatorInfo &info) :
            m_simulatorInfo(info)
        { }

        IAircraftModelLoader::~IAircraftModelLoader()
        {
            this->gracefulShutdown();
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
            m_cancelLoading = true;
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
