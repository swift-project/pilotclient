/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelcaches.h"
#include "blackmisc/logmessage.h"

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

            CAircraftModelList IMultiSimulatorModelCaches::getCurrentCachedModels() const
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                if (!sim.isSingleSimulator()) { return CAircraftModelList(); }
                return this->getCachedModels(sim);
            }

            bool IMultiSimulatorModelCaches::syncronizeCurrentCache()
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                if (!sim.isSingleSimulator()) { return false; }
                this->syncronizeCache(sim);
                return true;
            }

            CModelCaches::CModelCaches(QObject *parent) : IMultiSimulatorModelCaches(parent)
            {
                this->m_currentSimulator.synchronize();
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
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return CAircraftModelList();
                }
            }

            CStatusMessage CModelCaches::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                const CStatusMessage m = this->m_currentSimulator.set(simulator);
                if (m.isFailure()) { return m; }
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.set(models);
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.set(models);
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.set(models);
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.set(models);
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return CStatusMessage();
                }
            }

            QDateTime IMultiSimulatorModelCaches::getCurrentCacheTimestamp() const
            {
                const CSimulatorInfo sim(this->getCurrentSimulator());
                if (!sim.isSingleSimulator()) { return QDateTime(); }
                return this->getCacheTimestamp(sim);
            }

            QDateTime CModelCaches::getCacheTimestamp(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.getTimestamp();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.getTimestamp();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.getTimestamp();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.getTimestamp();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return QDateTime();
                }
            }

            void CModelCaches::syncronizeCache(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                const CStatusMessage m = this->m_currentSimulator.set(simulator);
                if (m.isFailure()) { CLogMessage::preformatted(m); }
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.synchronize(); break;
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.synchronize(); break;
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.synchronize(); break;
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.synchronize(); break;
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                }
            }

            void CModelCaches::setCurrentSimulator(const CSimulatorInfo &simulator)
            {
                this->syncronizeCache(simulator);
            }

            CModelSetCaches::CModelSetCaches(QObject *parent) : IMultiSimulatorModelCaches(parent)
            {
                this->m_currentSimulator.synchronize();
            }

            CAircraftModelList CModelSetCaches::getCachedModels(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.getCopy();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.getCopy();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.getCopy();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.getCopy();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return CAircraftModelList();
                }
            }

            CStatusMessage CModelSetCaches::setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                const CStatusMessage m = this->m_currentSimulator.set(simulator);
                if (m.isFailure()) { return m; }
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
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return CStatusMessage();
                }
            }

            QDateTime CModelSetCaches::getCacheTimestamp(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.getTimestamp();
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.getTimestamp();
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.getTimestamp();
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.getTimestamp();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    return QDateTime();
                }
            }

            void CModelSetCaches::syncronizeCache(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                const CStatusMessage m = this->m_currentSimulator.set(simulator);
                if (m.isFailure()) { CLogMessage::preformatted(m); }
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_modelCacheFs9.synchronize(); break;
                case CSimulatorInfo::FSX: return this->m_modelCacheFsx.synchronize(); break;
                case CSimulatorInfo::P3D: return this->m_modelCacheP3D.synchronize(); break;
                case CSimulatorInfo::XPLANE: return this->m_modelCacheXP.synchronize(); break;
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                }
            }

            void CModelSetCaches::setCurrentSimulator(const CSimulatorInfo &simulator)
            {
                this->syncronizeCache(simulator);
            }

        } // ns
    } // ns
} // ns
