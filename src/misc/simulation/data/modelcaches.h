// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_DATA_MODELCACHES
#define SWIFT_MISC_SIMULATION_DATA_MODELCACHES

#include <atomic>

#include <QDateTime>
#include <QObject>

#include "misc/applicationinfo.h"
#include "misc/datacache.h"
#include "misc/simulation/aircraftmodelinterfaces.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/threadutils.h"

namespace swift::misc::simulation::data
{
    //! Trait for model cache
    struct TModelCache : public TDataTrait<CAircraftModelList>
    {
        //! Defer loading
        static constexpr bool isDeferred() { return true; }
    };

    //! \name Caches for own models on disk, loaded by IAircraftModelLoader
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

    //! FG
    struct TModelCacheFG : public TModelCache
    {
        //! Key in data cache
        static const char *key() { return "modelcachefg"; }
    };

    //! MSFS
    struct TModelCacheMsfs : public TModelCache
    {
        //! Key in data cache
        static const char *key() { return "modelcachemsfs"; }
    };

    //! Last selection
    struct TModelCacheLastSelection : public TDataTrait<CSimulatorInfo>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Default simulator
        static const CSimulatorInfo &defaultValue() { return CSimulatorInfo::guessDefaultSimulator(); }

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

    //! FG
    struct TModelSetCacheFG : public TModelCache
    {
        //! Key in data cache
        static const char *key() { return "modelsetfg"; }
    };

    //! MSFS
    struct TModelSetCacheMsfs : public TModelCache
    {
        //! Key in data cache
        static const char *key() { return "modelsetmsfs"; }
    };

    //! Last selection
    struct TSimulatorLastSelection : public TDataTrait<CSimulatorInfo>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Default simulator
        static const CSimulatorInfo &defaultValue() { return CSimulatorInfo::guessDefaultSimulator(); }

        //! Key
        static const char *key() { return "simulatorlastselection"; }
    };

    //! Last selections
    struct TSimulatorLastSelections : public TDataTrait<CSimulatorInfo>
    {
        //! First load is synchronous
        static constexpr bool isPinned() { return true; }

        //! Default simulator
        static const CSimulatorInfo &defaultValue() { return CSimulatorInfo::guessDefaultSimulator(); }

        //! Key
        static const char *key() { return "simulatorlastselections"; }
    };
    //! @}

    //! Trait for vPilot derived models
    struct TVPilotAircraftModels : public TModelCache
    {
        //! Key in data cache
        static const char *key() { return "vpilot/models"; }
    };

    //! Cache for multiple simulators specified by CSimulatorInfo
    class SWIFT_MISC_EXPORT IMultiSimulatorModelCaches :
        public QObject,
        public IModelsForSimulatorSetable,
        public IModelsForSimulatorUpdatable
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorSetable)
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorUpdatable)

    public:
        //! \copydoc IModelsForSimulatorSetable::setModelsForSimulator
        void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override;

        //! \copydoc IModelsForSimulatorUpdatable::updateModelsForSimulator
        int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override;

        //! Models for simulator
        //! \threadsafe
        virtual CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const = 0;

        //! Models
        CAircraftModelList getSynchronizedCachedModels(const CSimulatorInfo &simulator);

        //! Count of models for simulator
        int getCachedModelsCount(const CSimulatorInfo &simulator) const;

        //! Get filename for simulator cache file
        virtual QString getFilename(const CSimulatorInfo &simulator) const = 0;

        //! Has the other version the file?
        bool hasOtherVersionFile(const swift::misc::CApplicationInfo &info, const CSimulatorInfo &simulator) const;

        //! Simulators of given other versionwhich have a cache file
        CSimulatorInfo otherVersionSimulatorsWithFile(const swift::misc::CApplicationInfo &info) const;

        //! All file names
        virtual QStringList getAllFilenames() const;

        //! Simulator which uses cache with filename
        CSimulatorInfo getSimulatorForFilename(const QString &filename) const;

        //! Cache timestamp
        //! \threadsafe
        virtual QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const = 0;

        //! Set cache timestamp
        //! \threadsafe
        virtual CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) = 0;

        //! Cache saved?
        //! \threadsafe
        virtual bool isSaved(const CSimulatorInfo &simulator) const = 0;

        //! Initialized caches for which simulator?
        //! \threadsafe
        CSimulatorInfo simulatorsWithInitializedCache() const;

        //! Simulators which have models
        //! \threadsafe
        CSimulatorInfo simulatorsWithModels() const;

        //! Timestamp
        //! \threadsafe
        QDateTime getSynchronizedTimestamp(const CSimulatorInfo &simulator);

        //! Set cached models
        //! \threadsafe
        virtual CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) = 0;

        //! Clear cached models
        //! \threadsafe
        virtual CStatusMessage clearCachedModels(const CSimulatorInfo &simulator);

        //! Synchronize for given simulator
        //! \threadsafe
        virtual void synchronizeCache(const CSimulatorInfo &simulator) = 0;

        //! Is the cache already synchronized?
        //! \threadsafe
        bool isCacheAlreadySynchronized(const CSimulatorInfo &simulator) const;

        //! Synchronize multiple simulators
        //! \threadsafe
        virtual void synchronizeMultiCaches(const CSimulatorInfo &simulator);

        //! Admit the cache for given simulator
        //! \threadsafe
        virtual void admitCache(const CSimulatorInfo &simulator) = 0;

        //! Synchronize multiple simulators
        //! \threadsafe
        virtual void admitMultiCaches(const CSimulatorInfo &simulator);

        //! Info string about models in cache
        //! \threadsafe
        QString getInfoString() const;

        //! Info string without XPlane (FSX,P3D, FS9)
        //! \threadsafe
        QString getInfoStringFsFamily() const;

        //! Cache count and timestamp
        //! \threadsafe
        QString getCacheCountAndTimestamp(const CSimulatorInfo &simulator) const;

        //! Graceful shutdown
        virtual void gracefulShutdown();

        //! Descriptive text
        virtual QString getDescription() const = 0;

    signals:
        //! Cache has been changed
        //! \note this detects caches changed elsewhere or set here (the normal caches detect only "elsewhere"
        void cacheChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

    protected:
        //! Construtor
        IMultiSimulatorModelCaches(QObject *parent = nullptr) : QObject(parent) {}

        //! Cache has been changed. This will only detect changes elsewhere, owned caches will not signal local changes
        //! @{
        void changedFsx() { this->emitCacheChanged(CSimulatorInfo::fsx()); }
        void changedFs9() { this->emitCacheChanged(CSimulatorInfo::fs9()); }
        void changedP3D() { this->emitCacheChanged(CSimulatorInfo::p3d()); }
        void changedXP() { this->emitCacheChanged(CSimulatorInfo::xplane()); }
        void changedFG() { this->emitCacheChanged(CSimulatorInfo::fg()); }
        void changedMsfs() { this->emitCacheChanged(CSimulatorInfo::msfs()); }
        //! @}

        //! Is the cache already synchronized?
        //! \threadsafe
        void markCacheAsAlreadySynchronized(const CSimulatorInfo &simulator, bool synchronized);

        //! Emit cacheChanged() utility function (allows breakpoint)
        void emitCacheChanged(const CSimulatorInfo &simulator);

    private:
        //! @{
        //! Cache synchronized flag
        std::atomic_bool m_syncFsx { false };
        std::atomic_bool m_syncP3D { false };
        std::atomic_bool m_syncFS9 { false };
        std::atomic_bool m_syncFG { false };
        std::atomic_bool m_syncXPlane { false };
        std::atomic_bool m_syncMsfs { false };
        //! @}
    };

    //! Bundle of caches for all simulators
    //! \remark remembers its last simulator selection
    class SWIFT_MISC_EXPORT CModelCaches : public IMultiSimulatorModelCaches
    {
        Q_OBJECT

    public:
        //! Construtor
        CModelCaches(bool synchronizeCache, QObject *parent = nullptr);

        //! Log categories
        static const QStringList &getLogCategories();

        //! \name Interface implementations
        //! @{
        CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const override;
        CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) override;
        QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const override;
        CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) override;
        void synchronizeCache(const CSimulatorInfo &simulator) override;
        void admitCache(const CSimulatorInfo &simulator) override;
        QString getFilename(const CSimulatorInfo &simulator) const override;
        bool isSaved(const CSimulatorInfo &simulator) const override;
        QString getDescription() const override { return "Model caches"; }
        //! @}

    private:
        CData<TModelCacheFsx> m_modelCacheFsx { this, &CModelCaches::changedFsx }; //!< FSX cache
        CData<TModelCacheFs9> m_modelCacheFs9 { this, &CModelCaches::changedFs9 }; //!< FS9 cache
        CData<TModelCacheP3D> m_modelCacheP3D { this, &CModelCaches::changedP3D }; //!< P3D cache
        CData<TModelCacheXP> m_modelCacheXP { this, &CModelCaches::changedXP }; //!< XP cache
        CData<TModelCacheFG> m_modelCacheFG { this, &CModelCaches::changedFG }; //!< XP cache
        CData<TModelCacheMsfs> m_modelCacheMsfs { this, &CModelCaches::changedMsfs }; //!< MSFS cache

        //! Non virtual version (can be used in ctor)
        void synchronizeCacheImpl(const CSimulatorInfo &simulator);

        //! Non virtual version (can be used in ctor)
        //! \threadsafe
        bool admitCacheImpl(const CSimulatorInfo &simulator);
    };

    //! Bundle of caches for model sets of all simulators
    //! \remark remembers its last simulator selection
    class SWIFT_MISC_EXPORT CModelSetCaches : public IMultiSimulatorModelCaches
    {
        Q_OBJECT

    public:
        //! Construtor
        CModelSetCaches(bool synchronizeCache, QObject *parent = nullptr);

        //! Log categories
        static const QStringList &getLogCategories();

        //! \name Interface implementations
        //! @{
        CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const override;
        CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) override;
        QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const override;
        CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) override;
        void synchronizeCache(const CSimulatorInfo &simulator) override;
        void admitCache(const CSimulatorInfo &simulator) override;
        QString getFilename(const CSimulatorInfo &simulator) const override;
        bool isSaved(const CSimulatorInfo &simulator) const override;
        QString getDescription() const override { return "Model sets"; }
        //! @}

    private:
        CData<TModelSetCacheFsx> m_modelCacheFsx { this, &CModelSetCaches::changedFsx }; //!< FSX cache
        CData<TModelSetCacheFs9> m_modelCacheFs9 { this, &CModelSetCaches::changedFs9 }; //!< FS9 cache
        CData<TModelSetCacheP3D> m_modelCacheP3D { this, &CModelSetCaches::changedP3D }; //!< P3D cache
        CData<TModelSetCacheXP> m_modelCacheXP { this, &CModelSetCaches::changedXP }; //!< XP cache
        CData<TModelSetCacheFG> m_modelCacheFG { this, &CModelSetCaches::changedFG }; //!< FG cache
        CData<TModelSetCacheMsfs> m_modelCacheMsfs { this, &CModelSetCaches::changedMsfs }; //!< MSFS cache

        //! Non virtual version (can be used in ctor)
        void synchronizeCacheImpl(const CSimulatorInfo &simulator);

        //! Non virtual version (can be used in ctor)
        //! \threadsafe
        bool admitCacheImpl(const CSimulatorInfo &simulator);
    };

    //! One central instance of the caches base class
    template <class TCaches, class Derived>
    class CCentralMultiSimulatorModelCachesProviderBase : public IMultiSimulatorModelCaches
    {
    public:
        //! \name Interface implementations
        //! @{

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::getCachedModels
        CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const override
        {
            return instanceCaches().getCachedModels(simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::setCachedModels
        CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) override
        {
            return instanceCaches().setCachedModels(models, simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::getCacheTimestamp
        QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const override
        {
            return instanceCaches().getCacheTimestamp(simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::setCacheTimestamp
        CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator) override
        {
            return instanceCaches().setCacheTimestamp(ts, simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::synchronizeCache
        void synchronizeCache(const CSimulatorInfo &simulator) override
        {
            return instanceCaches().synchronizeCache(simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::admitCache
        void admitCache(const CSimulatorInfo &simulator) override { return instanceCaches().admitCache(simulator); }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::getFilename
        QString getFilename(const CSimulatorInfo &simulator) const override
        {
            return instanceCaches().getFilename(simulator);
        }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::isSaved
        bool isSaved(const CSimulatorInfo &simulator) const override { return instanceCaches().isSaved(simulator); }

        //! \copydoc swift::misc::simulation::data::IMultiSimulatorModelCaches::getDescription
        QString getDescription() const override { return instanceCaches().getDescription(); }
        //! @}

    protected:
        //! Ctor
        CCentralMultiSimulatorModelCachesProviderBase(const QString &name, QObject *parent = nullptr)
            : IMultiSimulatorModelCaches(parent)
        {
            this->setObjectName(name);
            connect(&m_caches, &IMultiSimulatorModelCaches::cacheChanged, this,
                    &CCentralMultiSimulatorModelCachesProviderBase::cacheChanged);
        }

    private:
        TCaches m_caches { false, this }; //!< used caches

        //! Singleton caches
        TCaches &instanceCaches() { return this->isInstance() ? m_caches : Derived::modelCachesInstance().m_caches; }

        //! Singleton caches
        const TCaches &instanceCaches() const
        {
            return this->isInstance() ? m_caches : Derived::modelCachesInstance().m_caches;
        }

        //! Is this object the central instance?
        //! \remark would also allow do direct inherit this class
        bool isInstance() const { return this == &Derived::modelCachesInstance(); }
    };

    //! One central instance of the model caches
    class SWIFT_MISC_EXPORT CCentralMultiSimulatorModelCachesProvider :
        public CCentralMultiSimulatorModelCachesProviderBase<CModelCaches, CCentralMultiSimulatorModelCachesProvider>
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorSetable)
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorUpdatable)

    public:
        //! Central instance
        //! \remark can only be used in the thread created
        static CCentralMultiSimulatorModelCachesProvider &modelCachesInstance()
        {
            static CCentralMultiSimulatorModelCachesProvider c("Central model caches provider");
            Q_ASSERT_X(CThreadUtils::isInThisThread(&c), Q_FUNC_INFO, "Wrong thread");
            return c;
        }

    protected:
        //! Ctor
        CCentralMultiSimulatorModelCachesProvider(const QString &name, QObject *parent = nullptr)
            : CCentralMultiSimulatorModelCachesProviderBase(name, parent)
        {}
    };

    //! Basically a QObject free (delegate based) version of CCentralMultiSimulatorModelCachesProvider
    class SWIFT_MISC_EXPORT CCentralMultiSimulatorModelCachesAware :
        public IModelsForSimulatorSetable,
        public IModelsForSimulatorUpdatable
    {
    public:
        //! @{
        //! Look like IMultiSimulatorModelCaches interface
        CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getCachedModels(simulator);
        }
        int getCachedModelsCount(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getCachedModelsCount(simulator);
        }
        QString getCacheCountAndTimestamp(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getCacheCountAndTimestamp(
                simulator);
        }
        CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().setCachedModels(models, simulator);
        }
        CStatusMessage clearCachedModels(const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().clearCachedModels(simulator);
        }
        QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getCacheTimestamp(simulator);
        }
        CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().setCacheTimestamp(ts, simulator);
        }
        void synchronizeCache(const CSimulatorInfo &simulator)
        {
            CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        }
        void admitCache(const CSimulatorInfo &simulator)
        {
            CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().admitCache(simulator);
        }
        QString getFilename(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getFilename(simulator);
        }
        bool isSaved(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().isSaved(simulator);
        }
        QString getDescription() const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getDescription();
        }
        QString getInfoString() const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getInfoString();
        }
        QString getInfoStringFsFamily() const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getInfoStringFsFamily();
        }
        //! @}

        //! \copydoc IModelsForSimulatorSetable::setModelsForSimulator
        void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override
        {
            CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().setModelsForSimulator(models, simulator);
        }

        //! \copydoc IModelsForSimulatorUpdatable::updateModelsForSimulator
        int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().updateModelsForSimulator(models,
                                                                                                             simulator);
        }
    };

    //! One central instance of the model set caches
    class SWIFT_MISC_EXPORT CCentralMultiSimulatorModelSetCachesProvider :
        public CCentralMultiSimulatorModelCachesProviderBase<CModelSetCaches,
                                                             CCentralMultiSimulatorModelSetCachesProvider>
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorSetable)
        Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorUpdatable)

    public:
        //! Central instance
        //! \remark can only be used in the thread created
        static CCentralMultiSimulatorModelSetCachesProvider &modelCachesInstance()
        {
            static CCentralMultiSimulatorModelSetCachesProvider c("Central model sets provider");
            Q_ASSERT_X(CThreadUtils::isInThisThread(&c), Q_FUNC_INFO, "Wrong thread");
            return c;
        }

    protected:
        //! Ctor
        CCentralMultiSimulatorModelSetCachesProvider(const QString &name, QObject *parent = nullptr)
            : CCentralMultiSimulatorModelCachesProviderBase(name, parent)
        {}
    };

    //! Basically a QObject free (delegate based) version of CCentralMultiSimulatorModelSetCachesProvider
    class SWIFT_MISC_EXPORT CCentralMultiSimulatorModelSetCachesAware :
        public IModelsForSimulatorSetable,
        public IModelsForSimulatorUpdatable
    {
    public:
        //! @{
        //! Look like IMultiSimulatorModelCaches interface
        CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
        }
        int getCachedModelsCount(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModelsCount(simulator);
        }
        QString getCacheCountAndTimestamp(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getCacheCountAndTimestamp(
                simulator);
        }
        CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().setCachedModels(models,
                                                                                                       simulator);
        }
        CStatusMessage clearCachedModels(const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().clearCachedModels(simulator);
        }
        QDateTime getCacheTimestamp(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCacheTimestamp(simulator);
        }
        CStatusMessage setCacheTimestamp(const QDateTime &ts, const CSimulatorInfo &simulator)
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().setCacheTimestamp(ts, simulator);
        }
        void synchronizeCache(const CSimulatorInfo &simulator)
        {
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        }
        void admitCache(const CSimulatorInfo &simulator)
        {
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().admitCache(simulator);
        }
        QString getFilename(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getFilename(simulator);
        }
        bool isSaved(const CSimulatorInfo &simulator) const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().isSaved(simulator);
        }
        QString getDescription() const
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getDescription();
        }
        QString getInfoString() const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getInfoString();
        }
        QString getInfoStringFsFamily() const
        {
            return CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().getInfoStringFsFamily();
        }
        //! @}

        //! \copydoc IModelsForSimulatorSetable::setModelsForSimulator
        void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override
        {
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().setModelsForSimulator(models,
                                                                                                      simulator);
        }

        //! \copydoc IModelsForSimulatorUpdatable::updateModelsForSimulator
        int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override
        {
            return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().updateModelsForSimulator(
                models, simulator);
        }
    };

} // namespace swift::misc::simulation::data

#endif // SWIFT_MISC_SIMULATION_DATA_MODELCACHES
