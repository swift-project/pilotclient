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

        IAircraftModelLoader *IAircraftModelLoader::createModelLoader(const CSimulatorInfo &info)
        {
            //! \todo hack, remove later and replace by factory
            IAircraftModelLoader *ml = BlackMisc::Simulation::FsCommon::CAircraftCfgParser::createModelLoader(info);
            if (ml) { return ml; }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No model model loader for simulator");
            return nullptr;
        }
    } // ns
} // ns
