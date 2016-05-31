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
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QDateTime>
#include <QObject>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            //! Trait for model cache
            struct ModelCache : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
            {
                //! Defer loading
                static constexpr bool isDeferred() { return true; }

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

            //! Last selection
            struct ModelCacheLastSelection : public BlackMisc::CDataTrait<BlackMisc::Simulation::CSimulatorInfo>
            {
                //! Default value
                static const BlackMisc::Simulation::CSimulatorInfo &defaultValue()
                {
                    static const BlackMisc::Simulation::CSimulatorInfo s(BlackMisc::Simulation::CSimulatorInfo::guessDefaultSimulator());
                    return s;
                }

                //! First load is synchronous
                static constexpr bool isPinned() { return true; }

                //! Key
                static const char *key() { return "modelcachelastselection"; }
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

            //! Last selection
            struct ModelSetLastSelection : public BlackMisc::CDataTrait<BlackMisc::Simulation::CSimulatorInfo>
            {
                //! Default value
                static const BlackMisc::Simulation::CSimulatorInfo &defaultValue()
                {
                    static const BlackMisc::Simulation::CSimulatorInfo s(BlackMisc::Simulation::CSimulatorInfo::guessDefaultSimulator());
                    return s;
                }

                //! First load is synchronous
                static constexpr bool isPinned() { return true; }

                //! Key
                static const char *key() { return "modelsetlastselection"; }
            };
            //! @}

            //! Trait for vPilot derived models
            struct VPilotAircraftModels : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "vpilot/models"; }
            };

            //! Cache for multiple simulators specified by BlackMisc::Simulation::CSimulatorInfo
            class IMultiSimulatorModelCaches :
                public QObject,
                public IModelsPerSimulatorSetable
            {
                Q_OBJECT

            public:
                //! Construtor
                IMultiSimulatorModelCaches(QObject *parent = nullptr) : QObject(parent)
                { }

                //! Models
                //! \threadsafe
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! Models
                //! \todo is that threadsafe?
                CAircraftModelList getSyncronizedCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Models
                //! \threadsafe
                CAircraftModelList getCurrentCachedModels() const;

                //! Cache timestamp
                //! \threadsafe
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! Timestamp
                //! \todo is that threadsafe?
                QDateTime getSyncronizedTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Last selection`s timestamp
                //! \threadsafe
                QDateTime getCurrentCacheTimestamp() const;

                //! Set cache
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Syncronize
                //! \todo is that threadsafe?
                virtual void syncronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Last cache
                bool syncronizeCurrentCache();

                //! Selected simulator
                //! \threadsafe
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const = 0;

                //!Selected simulator
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! \copydoc IModelsPerSimulatorSetable::setModels
                virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

            signals:
                //! Cache has been changed
                void cacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            protected:
                //! \name Cache has been changed
                //! @{
                void changedFsx() { emitCacheChanged(BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::FSX)); }
                void changedFs9() { emitCacheChanged(BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::FS9)); }
                void changedP3D() { emitCacheChanged(BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::P3D)); }
                void changedXP()  { emitCacheChanged(BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::XPLANE)); }
                //! @}

            private:
                //! Emit cacheChanged() utility function (allows breakpoint)
                void emitCacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);
            };

            //! Bundle of caches for all simulators
            //! \remark remembers its last simulator selection
            class CModelCaches : public IMultiSimulatorModelCaches
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelCaches(QObject *parent = nullptr);

                //! \name Interface implementations
                //! @{
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual void syncronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const override { return this->m_currentSimulator.getCopy(); }
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                //! @}

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheFsx> m_modelCacheFsx {this, &CModelCaches::changedFsx }; //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheFs9> m_modelCacheFs9 {this, &CModelCaches::changedFs9 }; //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheP3D> m_modelCacheP3D {this, &CModelCaches::changedP3D }; //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheXP>  m_modelCacheXP  {this, &CModelCaches::changedXP };  //!< XP cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelCacheLastSelection> m_currentSimulator { this };              //!< current simulator
            };

            //! Bundle of caches for model sets of all simulators
            //! \remark remembers its last simulator selection
            class CModelSetCaches : public IMultiSimulatorModelCaches
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelSetCaches(QObject *parent = nullptr);

                //! \name Interface implementations
                //! @{
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual void syncronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const override { return this->m_currentSimulator.getCopy(); }
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                //! @}

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheFsx> m_modelCacheFsx {this, &CModelSetCaches::changedFsx };  //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheFs9> m_modelCacheFs9 {this, &CModelSetCaches::changedFs9};   //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheP3D> m_modelCacheP3D {this, &CModelSetCaches::changedP3D };  //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetCacheXP>  m_modelCacheXP  {this, &CModelSetCaches::changedXP };   //!< XP cache
                BlackMisc::CData<BlackMisc::Simulation::Data::ModelSetLastSelection> m_currentSimulator { this };                       //!< current simulator
            };
        } // ns
    } // ns
} // ns

#endif // guard
