/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackmisc/simulation/data/modelcaches.h"

#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            void IMultiSimulatorModelCaches::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                this->setCachedModels(models, simulator);
            }

            QString IMultiSimulatorModelCaches::getInfoString() const
            {
                static const QString is("FSX: %1 P3D: %2 FS9: %3 XP: %4");
                return is.arg(this->getCachedModelsCount(CSimulatorInfo::FSX)).arg(this->getCachedModelsCount(CSimulatorInfo::P3D)).arg(this->getCachedModelsCount(CSimulatorInfo::FS9)).arg(this->getCachedModelsCount(CSimulatorInfo::XPLANE));
            }

            QString IMultiSimulatorModelCaches::getInfoStringFsFamily() const
            {
                static const QString is("FSX: %1 P3D: %2 FS9: %3");
                return is.arg(this->getCachedModelsCount(CSimulatorInfo::FSX)).arg(this->getCachedModelsCount(CSimulatorInfo::P3D)).arg(this->getCachedModelsCount(CSimulatorInfo::FS9));
            }

            void IMultiSimulatorModelCaches::emitCacheChanged(const CSimulatorInfo &simulator)
            {
                emit this->cacheChanged(simulator);
            }

            int IMultiSimulatorModelCaches::getCachedModelsCount(const CSimulatorInfo &simulator) const
            {
                return this->getCachedModels(simulator).size();
            }

            CAircraftModelList IMultiSimulatorModelCaches::getSynchronizedCachedModels(const CSimulatorInfo &simulator)
            {
                BLACK_VERIFY_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                this->synchronizeCache(simulator);
                return this->getCachedModels(simulator);
            }

            CAircraftModelList IMultiSimulatorModelCaches::getCurrentCachedModels() const
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                BLACK_VERIFY_X(sim.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                if (!sim.isSingleSimulator()) { return CAircraftModelList(); }
                return this->getCachedModels(sim);
            }

            QDateTime IMultiSimulatorModelCaches::getSynchronizedTimestamp(const CSimulatorInfo &simulator)
            {
                BLACK_VERIFY_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                this->synchronizeCache(simulator);
                return this->getCacheTimestamp(simulator);
            }

            bool IMultiSimulatorModelCaches::synchronizeCurrentCache()
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                BLACK_VERIFY_X(sim.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                if (!sim.isSingleSimulator()) { return false; }
                this->synchronizeCache(sim);
                return true;
            }

            bool IMultiSimulatorModelCaches::admitCurrentCache()
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                BLACK_VERIFY_X(sim.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                if (!sim.isSingleSimulator()) { return false; }
                this->admitCache(sim);
                return true;
            }

            CModelCaches::CModelCaches(bool synchronizeCache, QObject *parent) : IMultiSimulatorModelCaches(parent)
            {
                this->m_currentSimulator.synchronize();
                CSimulatorInfo sim(this->m_currentSimulator.get());
                if (!sim.isSingleSimulator())
                {
                    sim = BlackMisc::Simulation::CSimulatorInfo::guessDefaultSimulator();
                    this->m_currentSimulator.set(sim);
                    CLogMessage(this).warning("Invalid simulator, reseting");
                }
                const QString simStr(sim.toQString(true));
                if (synchronizeCache)
                {
                    this->synchronizeCacheImpl(sim);
                    CLogMessage(this).info("Initialized model caches to %1") << simStr;
                }
                else
                {
                    this->admitCacheImpl(sim);
                    CLogMessage(this).info("Admit model caches to %1") << simStr;
                }
            }

            CAircraftModelList CModelCaches::getCachedModels(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.get();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.get();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.get();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.get();
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    return CAircraftModelList();
                }
            }

            CStatusMessage CModelCaches::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.set(models);
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.set(models);
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.set(models);
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.set(models);
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    return CStatusMessage();
                }
            }

            QDateTime IMultiSimulatorModelCaches::getCurrentCacheTimestamp() const
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                BLACK_VERIFY_X(sim.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
                if (!sim.isSingleSimulator()) { return QDateTime(); }
                return this->getCacheTimestamp(sim);
            }

            QDateTime CModelCaches::getCacheTimestamp(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.getAvailableTimestamp();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.getAvailableTimestamp();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.getAvailableTimestamp();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.getAvailableTimestamp();
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    return QDateTime();
                }
            }

            void CModelCaches::synchronizeCache(const CSimulatorInfo &simulator)
            {
                this->synchronizeCacheImpl(simulator);
            }

            void CModelCaches::admitCache(const CSimulatorInfo &simulator)
            {
                this->admitCacheImpl(simulator);
            }

            CStatusMessage CModelCaches::setCurrentSimulator(const CSimulatorInfo &simulator)
            {
                static const CStatusMessage sameSimMsg = CStatusMessage(this).info("Same simulator");
                const CSimulatorInfo s = this->m_currentSimulator.get();
                if (s == simulator) { return sameSimMsg; }
                const BlackMisc::CStatusMessage m = this->m_currentSimulator.set(simulator);
                this->synchronizeCache(simulator);
                return m;
            }

            void CModelCaches::synchronizeCacheImpl(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: this->m_modelCacheFs9.synchronize(); break;
                case CSimulatorInfo::FSX: this->m_modelCacheFsx.synchronize(); break;
                case CSimulatorInfo::P3D: this->m_modelCacheP3D.synchronize(); break;
                case CSimulatorInfo::XPLANE: this->m_modelCacheXP.synchronize(); break;
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    break;
                }
            }

            void CModelCaches::admitCacheImpl(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: this->m_modelCacheFs9.admit(); break;
                case CSimulatorInfo::FSX: this->m_modelCacheFsx.admit(); break;
                case CSimulatorInfo::P3D: this->m_modelCacheP3D.admit(); break;
                case CSimulatorInfo::XPLANE: this->m_modelCacheXP.admit(); break;
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    break;
                }
            }

            CModelSetCaches::CModelSetCaches(bool synchronizeCache, QObject *parent) : IMultiSimulatorModelCaches(parent)
            {
                this->m_currentSimulator.synchronize();
                CSimulatorInfo sim(this->m_currentSimulator.get());
                if (!sim.isSingleSimulator())
                {
                    sim = BlackMisc::Simulation::CSimulatorInfo::guessDefaultSimulator();
                    this->m_currentSimulator.set(sim);
                    CLogMessage(this).warning("Invalid simulator, reseting");
                }

                const QString simStr(sim.toQString(true));
                if (synchronizeCache)
                {
                    this->synchronizeCacheImpl(sim);
                    CLogMessage(this).info("Initialized model caches to %1") << simStr;
                }
                else
                {
                    this->admitCacheImpl(sim);
                    CLogMessage(this).info("Admit model caches to %1") << simStr;
                }
            }

            CAircraftModelList CModelSetCaches::getCachedModels(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.get();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.get();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.get();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.get();
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    return CAircraftModelList();
                }
            }

            CStatusMessage CModelSetCaches::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                CAircraftModelList orderedModels(models);
                if (orderedModels.needsOrder())
                {
                    orderedModels.resetOrder();
                }
                else
                {
                    orderedModels.sortAscendingByOrder();
                }

                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.set(orderedModels);
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.set(orderedModels);
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.set(orderedModels);
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.set(orderedModels);
                default:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "wrong simulator");
                    return CStatusMessage();
                }
            }

            QDateTime CModelSetCaches::getCacheTimestamp(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.getAvailableTimestamp();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.getAvailableTimestamp();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.getAvailableTimestamp();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.getAvailableTimestamp();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return QDateTime();
                }
            }

            void CModelSetCaches::synchronizeCache(const CSimulatorInfo &simulator)
            {
                this->synchronizeCacheImpl(simulator);
            }

            void CModelSetCaches::admitCache(const CSimulatorInfo &simulator)
            {
                this->admitCacheImpl(simulator);
            }

            CStatusMessage CModelSetCaches::setCurrentSimulator(const CSimulatorInfo &simulator)
            {
                static const CStatusMessage sameSimMsg = CStatusMessage(this).info("Same simulator");
                const CSimulatorInfo s = this->m_currentSimulator.get();
                if (s == simulator) { return sameSimMsg; }
                const BlackMisc::CStatusMessage m = this->m_currentSimulator.set(simulator);
                this->synchronizeCache(simulator);
                return m;
            }

            void CModelSetCaches::synchronizeCacheImpl(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: this->m_modelCacheFs9.synchronize(); break;
                case CSimulatorInfo::FSX: this->m_modelCacheFsx.synchronize(); break;
                case CSimulatorInfo::P3D: this->m_modelCacheP3D.synchronize(); break;
                case CSimulatorInfo::XPLANE: this->m_modelCacheXP.synchronize(); break;
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
            }

            void CModelSetCaches::admitCacheImpl(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: this->m_modelCacheFs9.admit(); break;
                case CSimulatorInfo::FSX: this->m_modelCacheFsx.admit(); break;
                case CSimulatorInfo::P3D: this->m_modelCacheP3D.admit(); break;
                case CSimulatorInfo::XPLANE: this->m_modelCacheXP.admit(); break;
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
            }
        } // ns
    } // ns
} // ns
