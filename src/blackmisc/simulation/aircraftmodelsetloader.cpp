/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodelsetloader.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Simulation::Data;

namespace BlackMisc
{
    namespace Simulation
    {
        CAircraftModelSetLoader::CAircraftModelSetLoader(QObject *parent) :
            QObject(parent)
        {
            connect(&this->m_caches, &CModelSetCaches::cacheChanged, this, &CAircraftModelSetLoader::cacheChanged);
        }

        CAircraftModelSetLoader::~CAircraftModelSetLoader()
        {
            this->gracefulShutdown();
        }

        CStatusMessage CAircraftModelSetLoader::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->m_caches.getCurrentSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simulator"); }
            const CStatusMessage m(this->m_caches.setCachedModels(models, sim));
            return m;
        }

        CStatusMessage CAircraftModelSetLoader::replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : this->m_caches.getCurrentSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simuataor"); }
            this->m_caches.synchronizeCache(sim);
            CAircraftModelList allModels(this->m_caches.getSynchronizedCachedModels(sim));
            const int c = allModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (c > 0)
            {
                return this->setCachedModels(models, sim);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data changed");
            }
        }

        void CAircraftModelSetLoader::changeSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
            if (this->getSimulator() == simulator) { return; }
            this->m_caches.setCurrentSimulator(simulator);
            this->m_caches.synchronizeCurrentCache();
            emit simulatorChanged(simulator);
        }

        CAircraftModelList CAircraftModelSetLoader::getAircraftModels() const
        {
            return this->m_caches.getCurrentCachedModels();
        }

        CAircraftModelList CAircraftModelSetLoader::getAircraftModels(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            return this->m_caches.getSynchronizedCachedModels(simulator);
        }

        int CAircraftModelSetLoader::getAircraftModelsCount() const
        {
            return getAircraftModels().size();
        }

        CAircraftModel CAircraftModelSetLoader::getModelForModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            return this->getAircraftModels().findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModelList CAircraftModelSetLoader::getCachedModels(const CSimulatorInfo &simulator) const
        {
            return this->m_caches.getCachedModels(simulator);
        }

        QDateTime CAircraftModelSetLoader::getCacheTimestamp() const
        {
            return this->m_caches.getCurrentCacheTimestamp();
        }

        bool CAircraftModelSetLoader::synchronizeCache()
        {
            return this->m_caches.synchronizeCurrentCache();
        }

        bool CAircraftModelSetLoader::admitCache()
        {
            return this->m_caches.admitCurrentCache();
        }

        bool CAircraftModelSetLoader::hasCachedData() const
        {
            return !this->m_caches.getCurrentCachedModels().isEmpty();
        }

        CStatusMessage CAircraftModelSetLoader::clearCache()
        {
            return this->setCachedModels(CAircraftModelList());
        }

        CSimulatorInfo CAircraftModelSetLoader::getSimulator() const
        {
            return this->m_caches.getCurrentSimulator();
        }

        QString CAircraftModelSetLoader::getSimulatorAsString() const
        {
            return this->getSimulator().toQString();
        }

        bool CAircraftModelSetLoader::supportsSimulator(const CSimulatorInfo &info)
        {
            return this->getSimulator().matchesAny(info);
        }

        CSimulatorInfo CAircraftModelSetLoader::simulatorsWithInitializedModelSet() const
        {
            return this->m_caches.simulatorsWithInitializedCache();
        }

        void CAircraftModelSetLoader::gracefulShutdown()
        {
            // void
        }

        QString CAircraftModelSetLoader::getInfoString() const
        {
            return this->m_caches.getInfoString();
        }

        QString CAircraftModelSetLoader::getInfoStringFsFamily() const
        {
            return this->m_caches.getInfoStringFsFamily();
        }
    } // ns
} // ns
