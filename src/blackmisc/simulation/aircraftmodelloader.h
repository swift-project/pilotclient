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
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/pixmap.h"
#include <QObject>
#include <atomic>
#include <memory>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Load the aircraft for a simulator
         */
        class BLACKMISC_EXPORT IAircraftModelLoader : public QObject
        {
            Q_OBJECT

        public:
            //! Parser mode
            enum LoadModeFlag
            {
                NotSet                = 0,
                LoadDirectly          = 1 << 0,   //!< load syncronously (blocking), normally for testing
                LoadInBackground      = 1 << 1,   //!< load in background, asnycronously
                CacheUntilNewer       = 1 << 2,   //!< use cache until newer data re available
                CacheFirst            = 1 << 3,   //!< always use cache (if it has data)
                CacheSkipped          = 1 << 4,   //!< ignore cache
                CacheOnly             = 1 << 5,   //!< only read cache, never load from disk
                InBackgroundWithCache = LoadInBackground | CacheFirst,   //!< Background, cached
                InBackgroundNoCache   = LoadInBackground | CacheSkipped  //!< Background, not cached
            };
            Q_DECLARE_FLAGS(LoadMode, LoadModeFlag)

            //! Destructor
            virtual ~IAircraftModelLoader();

            //! Start the loading process from disk.
            //! Optional DB models can be passed and used for data consolidation.
            void startLoading(LoadMode mode = InBackgroundWithCache, const CAircraftModelList &dbModels = {});

            //! Change the directory
            bool changeRootDirectory(const QString &directory);

            //! Current root directory
            QString getRootDirectory() const { return this->m_rootDirectory; }

            //! Loading finished?
            virtual bool isLoadingFinished() const = 0;

            //! The loaded models
            BlackMisc::Simulation::CAircraftModelList getAircraftModels() const;

            //! Count of loaded models
            int getAircraftModelsCount() const { return getAircraftModels().size(); }

            //! Model files updated?
            virtual bool areModelFilesUpdated() const = 0;

            //! A representive pixmap for given model
            virtual BlackMisc::CPixmap iconForModel(const QString &modelName, BlackMisc::CStatusMessage &statusMessage) const = 0;

            //! Which simulator is supported by that very loader
            const BlackMisc::Simulation::CSimulatorInfo &getSimulator() const;

            //! Supported simulators as string
            QString getSimulatorAsString() const;

            //! Is the given simulator supported?
            bool supportsSimulator(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Cancel read
            void cancelLoading();

            //! Shutdown
            void gracefulShutdown();

            //! Create a loader and syncronize caches
            static std::unique_ptr<IAircraftModelLoader> createModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            //! Merge with DB data if possible
            static bool mergeWithDbData(BlackMisc::Simulation::CAircraftModelList &modelsFromSimulator, const BlackMisc::Simulation::CAircraftModelList &dbModels, bool force = false);

        public slots:
            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere nd update the cache with manipulated data.
            BlackMisc::CStatusMessage replaceCacheWithModelData(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere nd update the cache with manipulated data.
            BlackMisc::CStatusMessage replaceOrAddModelsInCache(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

        signals:
            //! Parsing is finished
            void loadingFinished(bool success, const BlackMisc::Simulation::CSimulatorInfo &simulator);

        protected:
            //! Constructor
            IAircraftModelLoader(const CSimulatorInfo &info, const QString &rootDirectory, const QStringList &excludeDirs = {});

            //! Cache timestamp
            QDateTime getCacheTimestamp() const;

            //! Make sure cache is syncronized
            void syncronizeCache();

            //! Any cached data?
            bool hasCachedData() const;

            //! Set models in cache
            BlackMisc::CStatusMessage setModelsInCache(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Clear cache
            BlackMisc::CStatusMessage clearCache();

            //! Check if directory exists
            bool existsDir(const QString &directory) const;

            //! Start the loading process from disk
            virtual void startLoadingFromDisk(LoadMode mode, const BlackMisc::Simulation::CAircraftModelList &dbModels) = 0;

            BlackMisc::Simulation::CSimulatorInfo m_simulatorInfo;       //!< Corresponding simulator
            std::atomic<bool> m_cancelLoading { false };                 //!< flag
            std::atomic<bool> m_loadingInProgress { false };             //!< Loading in progress
            QString           m_rootDirectory;                           //!< root directory parsing aircraft.cfg files
            QStringList       m_excludedDirectories;                     //!< directories not to be parsed
            BlackMisc::Simulation::Data::CModelCaches m_caches { this }; //!< caches

        protected slots:
            //! Loading finished
            void ps_loadFinished(bool success);
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::IAircraftModelLoader::LoadMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::IAircraftModelLoader::LoadModeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::IAircraftModelLoader::LoadMode)

#endif // guard
