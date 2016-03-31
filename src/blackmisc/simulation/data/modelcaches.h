/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_DATA_MODELCACHES
#define BLACKMISC_SIMULATION_DATA_MODELCACHES

#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            //! Trait for model cache
            struct ModelCache : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
            {
                //! Default value
                static const BlackMisc::Simulation::CAircraftModelList &defaultValue()
                {
                    static const BlackMisc::Simulation::CAircraftModelList ml;
                    return ml;
                }
            };

            //! \name Caches for own models on disk, loaded by BlackMisc::Simulation::IAircraftModelLoader
            //! @{

            //! XPlane
            struct ModelCacheXP : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachexp"; }
            };

            //! FSX
            struct ModelCacheFsx : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefsx"; }
            };

            //! FS9
            struct ModelCacheFs9 : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefs9"; }
            };

            //! P3D
            struct ModelCacheP3D : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachep3d"; }
            };
            //! @}


            //! \name Caches for choosen model sets
            //! @{

            //! XPlane
            struct ModelSetCacheXP : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetxp"; }
            };

            //! FSX
            struct ModelSetCacheFsx : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetfsx"; }
            };

            //! FS9
            struct ModelSetCacheFs9 : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetfs9"; }
            };

            //! P3D
            struct ModelSetCacheP3D : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetp3d"; }
            };
            //! @}


            //! Trait for vPilot derived models
            struct VPilotAircraftModels : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "vpilot/models"; }
            };

            //! Bundle of caches for all simulators
            //! \remark Temp. workaround
            class CModelCaches : public QObject
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelCaches(QObject *parent = nullptr);

                //! Models
                //! \threadsafe
                CAircraftModelList getModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Set models
                BlackMisc::CStatusMessage setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Cache timestamp
                //! \threadsafe
                QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Syncronize
                void syncronize(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheFsx> m_modelCacheFsx {this };  //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheFs9> m_modelCacheFs9 {this };  //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheP3D> m_modelCacheP3D {this };  //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheXP>  m_modelCacheXP  {this };  //!< XP cache
            };


            //! Bundle of caches for model sets of all simulators
            //! \remark Temp. workaround
            class CModelSetCaches : public QObject
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelSetCaches(QObject *parent = nullptr);

                //! Models
                //! \threadsafe
                CAircraftModelList getModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Set models
                BlackMisc::CStatusMessage setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Cache timestamp
                //! \threadsafe
                QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Syncronize
                void syncronize(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheFsx> m_modelCacheFsx {this };  //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheFs9> m_modelCacheFs9 {this };  //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheP3D> m_modelCacheP3D {this };  //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheXP>  m_modelCacheXP  {this };  //!< XP cache
            };

        } // ns
    } // ns
} // ns

#endif // guard
