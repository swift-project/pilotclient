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
        CAircraftModelSetLoader::CAircraftModelSetLoader(QObject *parent) : QObject(parent)
        {
            connect(&m_caches, &CModelSetCaches::cacheChanged, this, &CAircraftModelSetLoader::onModelsCacheChanged);
        }

        CAircraftModelSetLoader::~CAircraftModelSetLoader()
        {
            this->gracefulShutdown();
        }

        CStatusMessage CAircraftModelSetLoader::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : m_caches.getCurrentSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simulator"); }
            const CStatusMessage m(m_caches.setCachedModels(models, sim));
            return m;
        }

        CStatusMessage CAircraftModelSetLoader::replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data"); }
            const CSimulatorInfo sim = simulator.isSingleSimulator() ? simulator : m_caches.getCurrentSimulator();
            if (!sim.isSingleSimulator()) { return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid simuataor"); }
            m_caches.synchronizeCache(sim);
            CAircraftModelList allModels(m_caches.getSynchronizedCachedModels(sim));
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
            m_caches.setCurrentSimulator(simulator);
            m_caches.synchronizeCurrentCache();
            emit this->simulatorChanged(simulator);
        }

        CAircraftModelList CAircraftModelSetLoader::getAircraftModels() const
        {
            return m_caches.getCurrentCachedModels();
        }

        CAircraftModelList CAircraftModelSetLoader::getAircraftModels(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            return m_caches.getSynchronizedCachedModels(simulator);
        }

        int CAircraftModelSetLoader::getAircraftModelsCount() const
        {
            return this->getAircraftModels().size();
        }

        CAircraftModel CAircraftModelSetLoader::getModelForModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            return this->getAircraftModels().findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModelList CAircraftModelSetLoader::getCachedModels(const CSimulatorInfo &simulator) const
        {
            return m_caches.getCachedModels(simulator);
        }

        QDateTime CAircraftModelSetLoader::getCacheTimestamp() const
        {
            return m_caches.getCurrentCacheTimestamp();
        }

        bool CAircraftModelSetLoader::synchronizeCache()
        {
            return m_caches.synchronizeCurrentCache();
        }

        bool CAircraftModelSetLoader::admitCache()
        {
            return m_caches.admitCurrentCache();
        }

        bool CAircraftModelSetLoader::hasCachedData() const
        {
            return !m_caches.getCurrentCachedModels().isEmpty();
        }

        CStatusMessage CAircraftModelSetLoader::clearCache()
        {
            return this->setCachedModels(CAircraftModelList());
        }

        void CAircraftModelSetLoader::onModelsCacheChanged(const CSimulatorInfo &simulator)
        {
            this->changeSimulator(simulator);
            emit this->cacheChanged(simulator);
        }

        CSimulatorInfo CAircraftModelSetLoader::getSimulator() const
        {
            return m_caches.getCurrentSimulator();
        }

        QString CAircraftModelSetLoader::getSimulatorAsString() const
        {
            return this->getSimulator().toQString();
        }

        void CAircraftModelSetLoader::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Only one simulator per loader");
            if (this->getSimulator() == simulator) { return; }
            this->changeSimulator(simulator);
        }

        bool CAircraftModelSetLoader::supportsSimulator(const CSimulatorInfo &info)
        {
            return this->getSimulator().matchesAny(info);
        }

        CSimulatorInfo CAircraftModelSetLoader::simulatorsWithInitializedModelSet() const
        {
            return m_caches.simulatorsWithInitializedCache();
        }

        void CAircraftModelSetLoader::gracefulShutdown()
        {
            // void
        }

        QString CAircraftModelSetLoader::getInfoString() const
        {
            return m_caches.getInfoString();
        }

        QString CAircraftModelSetLoader::getInfoStringFsFamily() const
        {
            return m_caches.getInfoStringFsFamily();
        }
    } // ns
} // ns
