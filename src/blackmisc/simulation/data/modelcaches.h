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

#include "blackmisc/blackmiscexport.h"
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
            struct TModelCache : public TDataTrait<CAircraftModelList>
            {
                //! Defer loading
                static constexpr bool isDeferred() { return true; }
            };

            //! \name Caches for own models on disk, loaded by BlackMisc::Simulation::IAircraftModelLoader
            //! @{

            //! XPlane
            struct TModelCacheXP : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachexp"; }
            };

            //! FSX
            struct TModelCacheFsx : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefsx"; }
            };

            //! FS9
            struct TModelCacheFs9 : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefs9"; }
            };

            //! P3D
            struct TModelCacheP3D : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachep3d"; }
            };

            //! Last selection
            struct TModelCacheLastSelection : public BlackMisc::TDataTrait<BlackMisc::Simulation::CSimulatorInfo>
            {
                //! First load is synchronous
                static constexpr bool isPinned() { return true; }

                //! Default simulator
                static const BlackMisc::Simulation::CSimulatorInfo &defaultValue() { return CSimulatorInfo::guessDefaultSimulator(); }

                //! Key
                static const char *key() { return "modelcachelastselection"; }
            };
            //! @}

            //! \name Caches for choosen model sets
            //! @{

            //! XPlane
            struct TModelSetCacheXP : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetxp"; }
            };

            //! FSX
            struct TModelSetCacheFsx : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetfsx"; }
            };

            //! FS9
            struct TModelSetCacheFs9 : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetfs9"; }
            };

            //! P3D
            struct TModelSetCacheP3D : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelsetp3d"; }
            };

            //! Last selection
            struct TModelSetLastSelection : public BlackMisc::TDataTrait<BlackMisc::Simulation::CSimulatorInfo>
            {
                //! First load is synchronous
                static constexpr bool isPinned() { return true; }

                //! Default simulator
                static const BlackMisc::Simulation::CSimulatorInfo &defaultValue() { return CSimulatorInfo::guessDefaultSimulator(); }

                //! Key
                static const char *key() { return "modelsetlastselection"; }
            };
            //! @}

            //! Trait for vPilot derived models
            struct TVPilotAircraftModels : public TModelCache
            {
                //! Key in data cache
                static const char *key() { return "vpilot/models"; }
            };

            //! Cache for multiple simulators specified by BlackMisc::Simulation::CSimulatorInfo
            class BLACKMISC_EXPORT IMultiSimulatorModelCaches :
                public QObject,
                public IModelsPerSimulatorSetable
            {
                Q_OBJECT

            public:
                //! Models for simulator
                //! \threadsafe
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! Count of models for simulator
                int getCachedModelsCount(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Get filename for simulator cache file
                virtual QString getFilename(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! All file names
                virtual QStringList getAllFilenames() const;

                //! Simulator which uses cache with filename
                BlackMisc::Simulation::CSimulatorInfo getSimulatorForFilename(const QString &filename) const;

                //! Models
                CAircraftModelList getSynchronizedCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Models
                //! \threadsafe
                CAircraftModelList getCurrentCachedModels() const;

                //! Cache timestamp
                //! \threadsafe
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! Set cache timestamp
                virtual BlackMisc::CStatusMessage setCacheTimestamp(const QDateTime &ts, const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Cache saved?
                //! \threadsafe
                virtual bool isSaved(const BlackMisc::Simulation::CSimulatorInfo &simulator) const = 0;

                //! Initialized caches for which simulator?
                //! \threadsafe
                BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedCache() const;

                //! Timestamp
                QDateTime getSynchronizedTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Last selection`s timestamp
                //! \threadsafe
                QDateTime getCurrentCacheTimestamp() const;

                //! Set cache
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Synchronize for given simulator
                virtual void synchronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Synchronize the current
                bool synchronizeCurrentCache();

                //! Admit the cache for given simulator
                //! \threadsafe
                virtual void admitCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! Admit the current cache
                //! \threadsafe
                virtual bool admitCurrentCache();

                //! Selected simulator
                //! \threadsafe
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const = 0;

                //! Selected simulator
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

                //! \copydoc IModelsPerSimulatorSetable::setModels
                virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;

                //! Info string about models in cache
                QString getInfoString() const;

                //! Info string without XPlane (FSX,P3D, FS9)
                QString getInfoStringFsFamily() const;

                //! Descriptive text
                virtual QString getDescription() const = 0;

            signals:
                //! Cache has been changed
                void cacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            protected:
                //! Construtor
                IMultiSimulatorModelCaches(QObject *parent = nullptr) : QObject(parent)
                { }

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
            class BLACKMISC_EXPORT CModelCaches : public IMultiSimulatorModelCaches
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelCaches(bool synchronizeCache, QObject *parent = nullptr);

                //! Log categories
                static const BlackMisc::CLogCategoryList &getLogCategories();

                //! \name Interface implementations
                //! @{
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) override;
                virtual void synchronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual void admitCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const override { return m_currentSimulator.get(); }
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QString getFilename(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual bool isSaved(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual QString getDescription() const override { return "Model caches"; }
                //! @}

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheFsx> m_modelCacheFsx {this, &CModelCaches::changedFsx }; //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheFs9> m_modelCacheFs9 {this, &CModelCaches::changedFs9 }; //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheP3D> m_modelCacheP3D {this, &CModelCaches::changedP3D }; //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheXP>  m_modelCacheXP  {this, &CModelCaches::changedXP };  //!< XP cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheLastSelection> m_currentSimulator { this };              //!< current simulator

                //! Non virtual version (can be used in ctor)
                void synchronizeCacheImpl(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Non virtual version (can be used in ctor)
                //! \threadsafe
                void admitCacheImpl(const BlackMisc::Simulation::CSimulatorInfo &simulator);
            };

            //! Bundle of caches for model sets of all simulators
            //! \remark remembers its last simulator selection
            class BLACKMISC_EXPORT CModelSetCaches : public IMultiSimulatorModelCaches
            {
                Q_OBJECT

            public:
                //! Construtor
                CModelSetCaches(bool synchronizeCache, QObject *parent = nullptr);

                //! Log categories
                static const BlackMisc::CLogCategoryList &getLogCategories();

                //! \name Interface implementations
                //! @{
                virtual CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual BlackMisc::CStatusMessage setCachedModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QDateTime getCacheTimestamp(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) override;
                virtual void synchronizeCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual void admitCache(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual BlackMisc::Simulation::CSimulatorInfo getCurrentSimulator() const override { return m_currentSimulator.get(); }
                virtual BlackMisc::CStatusMessage setCurrentSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
                virtual QString getFilename(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual bool isSaved(const BlackMisc::Simulation::CSimulatorInfo &simulator) const override;
                virtual QString getDescription() const override { return "Model sets"; }
                //! @}

            private:
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheFsx> m_modelCacheFsx {this, &CModelSetCaches::changedFsx };  //!< FSX cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheFs9> m_modelCacheFs9 {this, &CModelSetCaches::changedFs9};   //!< FS9 cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheP3D> m_modelCacheP3D {this, &CModelSetCaches::changedP3D };  //!< P3D cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheXP>  m_modelCacheXP  {this, &CModelSetCaches::changedXP };   //!< XP cache
                BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetLastSelection> m_currentSimulator { this };                       //!< current simulator

                //! Non virtual version (can be used in ctor)
                void synchronizeCacheImpl(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Non virtual version (can be used in ctor)
                //! \threadsafe
                void admitCacheImpl(const BlackMisc::Simulation::CSimulatorInfo &simulator);
            };
        } // ns
    } // ns
} // ns

#endif // guard
