/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelcaches.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            CModelCaches::CModelCaches(QObject *parent) : QObject(parent)
            { }

            CAircraftModelList CModelCaches::getModels(const CSimulatorInfo &simulator) const
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

            CStatusMessage CModelCaches::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
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

            void CModelCaches::syncronize(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
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

            CModelSetCaches::CModelSetCaches(QObject *parent) : QObject(parent)
            { }

            CAircraftModelList CModelSetCaches::getModels(const CSimulatorInfo &simulator) const
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

            CStatusMessage CModelSetCaches::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
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

            void CModelSetCaches::syncronize(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
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

        } // ns
    } // ns
} // ns
