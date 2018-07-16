/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_IAIRCRAFTMODELLOADER_H
#define BLACKMISC_SIMULATION_IAIRCRAFTMODELLOADER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessagelist.h"

#include <QDateTime>
#include <QFlags>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>
#include <atomic>
#include <memory>
#include <functional>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Load the aircraft for a simulator
         */
        class BLACKMISC_EXPORT IAircraftModelLoader :
            public QObject,
            public IModelsSetable,
            public IModelsUpdatable,
            public IModelsPerSimulatorSetable,
            public IModelsPerSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorUpdatable)

        public:
            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Parser mode
            enum LoadModeFlag
            {
                NotSet                = 0,
                LoadDirectly          = 1 << 0,   //!< load synchronously (blocking), normally for testing
                LoadInBackground      = 1 << 1,   //!< load in background, asyncronously
                CacheFirst            = 1 << 2,   //!< always use cache (if it has data)
                CacheSkipped          = 1 << 3,   //!< ignore cache
                CacheOnly             = 1 << 4,   //!< only read cache, never load from disk
                InBackgroundWithCache = LoadInBackground | CacheFirst,   //!< Background, cached
                InBackgroundNoCache   = LoadInBackground | CacheSkipped  //!< Background, not checking cache
            };
            Q_DECLARE_FLAGS(LoadMode, LoadModeFlag)

            //! Load mode
            enum LoadFinishedInfo
            {
                CacheLoaded,   //!< cache was loaded
                ParsedData,    //!< parsed data
                LoadingSkipped //!< Loading skipped (empty directory)
            };

            //! Enum as string
            static QString enumToString(LoadFinishedInfo info);

            //! Enum as string
            static QString enumToString(LoadModeFlag modeFlag);

            //! Enum as string
            static QString enumToString(LoadMode mode);

            //! Destructor
            virtual ~IAircraftModelLoader();

            //! Callback to consolidate data, normally with DB data
            //! \remark this has to be a abstarct, as DB handling is subject of BlackCore
            using ModelConsolidationCallback = std::function<int (BlackMisc::Simulation::CAircraftModelList &, bool)>;

            //! Start the loading process from disk.
            //! Optional DB models can be passed and used for data consolidation.
            void startLoading(LoadMode mode = InBackgroundWithCache, const ModelConsolidationCallback &modelConsolidation = {}, const QStringList &modelDirectories = {});

            //! Loading finished?
            virtual bool isLoadingFinished() const = 0;

            //! Loading in progress
            //! \threadsafe
            bool isLoadingInProgress() const { return m_loadingInProgress; }

            //! Get the loaded models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getAircraftModels() const;

            //! Get the cached models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getCachedAircraftModels(const CSimulatorInfo &simulator) const;

            //! Count of loaded models
            //! \threadsafe
            int getAircraftModelsCount() const { return getAircraftModels().size(); }

            //! Skip the loading of empty (model) directories
            //! \remark loading of empty directories might erase the cache and is normally disable
            //! \threadsafe
            void setSkipLoadingOfEmptyDirectories(bool skip);

            //! Which simulator is supported by that very loader
            const CSimulatorInfo getSimulator() const;

            //! Supported simulators as string
            QString getSimulatorAsString() const;

            //! Is the given simulator supported?
            bool supportsSimulator(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Model directories
            QStringList getModelDirectoriesOrDefault() const;

            //! First directory, can be used when only 1 directory is expected
            QString getFirstModelDirectoryOrDefault() const;

            //! Exclude directories
            QStringList getModelExcludeDirectoryPatterns() const;

            //! Cancel loading
            void cancelLoading();

            //! Shutdown
            void gracefulShutdown();

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoString
            QString getModelCacheInfoString() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoStringFsFamily
            QString getModelCacheInfoStringFsFamily() const;

            //! \copydoc BlackMisc::Simulation::Data::IMultiSimulatorModelCaches::synchronizeCache
            void synchronizeModelCache(const CSimulatorInfo &simulator);

            //! \copydoc Settings::CMultiSimulatorSettings::getSpecializedSettings
            Settings::CSpecializedSimulatorSettings getCurrentSimulatorSettings() const;

            //! Access to multi simulator settings
            const Settings::CMultiSimulatorSettings &multiSimulatorSettings() const { return m_settings; }

            //! Access to multi simulator settings
            Settings::CMultiSimulatorSettings &multiSimulatorSettings() { return m_settings; }

            //! \name Implementations of the model interfaces (allows to set models modified in utility functions)
            //! @{
            virtual void setModels(const CAircraftModelList &models) override  { this->setCachedModels(models, this->getSimulator()); }
            virtual void updateModels(const CAircraftModelList &models) override  { this->replaceOrAddCachedModels(models, this->getSimulator()); }
            virtual void setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) override  { this->setCachedModels(models, simulator); }
            virtual void updateModels(const CAircraftModelList &models, const CSimulatorInfo &simulator) override  { this->replaceOrAddCachedModels(models, simulator); }
            //! @}

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            //! Normally used to consoidate data with DB data and write them back
            BlackMisc::CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            //! Normally used to consoidate data with DB data and write them back
            BlackMisc::CStatusMessage replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Create a loader and synchronize caches
            static std::unique_ptr<IAircraftModelLoader> createModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        signals:
            //! Parsing is finished or cache has been loaded
            void loadingFinished(const CStatusMessageList &status, const CSimulatorInfo &simulator, LoadFinishedInfo info);

            //! Corresponding Settings::CMultiSimulatorSettings::simulatorSettingsChange
            void simulatorSettingsChanged(const CSimulatorInfo &simulator);

        protected:
            //! Constructor
            IAircraftModelLoader(const CSimulatorInfo &simulator);

            //! Cache timestamp
            QDateTime getCacheTimestamp() const;

            //! Any cached data?
            bool hasCachedData() const;

            //! Clear cache
            BlackMisc::CStatusMessage clearCache();

            //! Start the loading process from disk
            virtual void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories) = 0;

            //! Loading finished, also logs messages
            void onLoadingFinished(const CStatusMessageList &statusMsgs, const CSimulatorInfo &simulator, LoadFinishedInfo info);

            //! A cache has been changed
            void onCacheChanged(const CSimulatorInfo &simInfo);

            //! A setting has been changed
            void onSettingsChanged(const CSimulatorInfo &simInfo);

            //! Get model directories from settings if empty, otherwise checked and UNC path fixed
            QStringList getInitializedModelDirectories(const QStringList &modelDirectories, const CSimulatorInfo &simulator) const;

            std::atomic<bool>  m_cancelLoading { false };           //!< flag, requesting to cancel loading
            std::atomic<bool>  m_loadingInProgress { false };       //!< loading in progress
            std::atomic<bool>  m_skipLoadingEmptyModelDir { true }; //!< loading empty model dirs might erase the cache, so normally we skip it
            CStatusMessageList m_loadingMessages;                   //!< loading messages
            Data::CModelCaches m_caches { false, this };            //!< caches used with this loader
            Settings::CMultiSimulatorSettings m_settings { this };  //!< settings

        private:
            void setObjectInfo(const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo);
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::IAircraftModelLoader::LoadMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::IAircraftModelLoader::LoadModeFlag)
Q_DECLARE_METATYPE(BlackMisc::Simulation::IAircraftModelLoader::LoadFinishedInfo)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::IAircraftModelLoader::LoadMode)

#endif // guard
