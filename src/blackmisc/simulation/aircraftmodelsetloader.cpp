/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelsetloader.h"

namespace BlackMisc
{
    namespace Simulation
    {
        CModelSetLoader::CModelSetLoader(const CSimulatorInfo &info, QObject *parent) :
            QObject(parent),
            m_simulatorInfo(info)
        {
            Q_ASSERT_X(info.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
        }

        CModelSetLoader::~CModelSetLoader()
        {
            this->gracefulShutdown();
        }

        CStatusMessage CModelSetLoader::setModelsInCache(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->m_simulatorInfo;
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simulator"); }
            const CStatusMessage m(this->m_caches.setModels(models, sim));
            return m;
        }

        CStatusMessage CModelSetLoader::replaceOrAddModelsInCache(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->m_simulatorInfo;
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simuataor"); }
            CAircraftModelList allModels(this->m_caches.getModels(sim));
            int c = allModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (c > 0)
            {
                return this->setModelsInCache(models, sim);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data changed");
            }
        }

        void CModelSetLoader::changeSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
            this->m_caches.syncronize(simulator);
            this->m_simulatorInfo = simulator;
            emit simulatorChanged(simulator);
        }

        CAircraftModelList CModelSetLoader::getAircraftModels() const
        {
            return this->m_caches.getModels(this->m_simulatorInfo);
        }

        QDateTime CModelSetLoader::getCacheTimestamp() const
        {
            return this->m_caches.getCacheTimestamp(this->m_simulatorInfo);
        }

        void CModelSetLoader::syncronizeCache()
        {
            return this->m_caches.syncronize(this->m_simulatorInfo);
        }

        bool CModelSetLoader::hasCachedData() const
        {
            return !this->m_caches.getModels(this->m_simulatorInfo).isEmpty();
        }

        CStatusMessage CModelSetLoader::clearCache()
        {
            return this->setModelsInCache(CAircraftModelList());
        }

        const CSimulatorInfo &CModelSetLoader::getSimulator() const
        {
            return m_simulatorInfo;
        }

        QString CModelSetLoader::getSimulatorAsString() const
        {
            return m_simulatorInfo.toQString();
        }

        bool CModelSetLoader::supportsSimulator(const CSimulatorInfo &info)
        {
            return getSimulator().matchesAny(info);
        }

        void CModelSetLoader::gracefulShutdown()
        {
            // void
        }
    } // ns
} // ns
