/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELSETLOADER_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELSETLOADER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QDateTime>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Handling of current set for simulator
         */
        class BLACKMISC_EXPORT CAircraftModelSetLoader :
            public QObject,
            public IModelsSetable,
            public IModelsUpdatable,
            public IModelsForSimulatorSetable,
            public IModelsForSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorUpdatable)

        public:
            //! Constructor
            CAircraftModelSetLoader(QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftModelSetLoader();

            //! Make sure cache is synchronized
            void synchronizeCache();

            //! Admit current cache
            void admitCache();

            //! The loaded models
            //! \threadsafe
            CAircraftModelList getAircraftModels() const;

            //! The loaded models for given simulator
            //! \threadsafe
            //! \remark non-const because it synchronizes cache
            CAircraftModelList getAircraftModels(const CSimulatorInfo &simulator);

            //! Count of loaded models
            //! \threadsafe
            int getAircraftModelsCount() const;

            //! Model for given model string
            //! \threadsafe
            CAircraftModel getModelForModelString(const QString &modelString) const;

            //! Models from cache
            //! \threadsafe
            CAircraftModelList getCachedModels(const CSimulatorInfo &simulator) const;

            //! Current simulator
            //! \threadsafe
            CSimulatorInfo getSimulator() const { return m_currentSimulator; }

            //! Supported simulators as string
            QString getSimulatorAsString() const;

            //! Set simulator
            //! \remark checked version, does nothing if simulator is alread set
            void setSimulator(const CSimulatorInfo &simulator);

            //! Is the given simulator supported?
            bool supportsSimulator(const CSimulatorInfo &info);

            //! Simulators with initialized caches
            CSimulatorInfo simulatorsWithInitializedModelSet() const;

            //! Shutdown
            void gracefulShutdown();

            //! \copydoc Data::CModelCaches::getInfoString
            QString getInfoString() const;

            //! \copydoc Data::CModelCaches::getInfoStringFsFamily
            QString getInfoStringFsFamily() const;

            //! \copydoc Data::CModelCaches::getCacheCountAndTimestamp
            QString getModelCacheCountAndTimestamp() const;

            //! \copydoc Data::CModelCaches::getCacheCountAndTimestamp
            QString getModelCacheCountAndTimestamp(const CSimulatorInfo &simulator) const;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const CAircraftModelList &models) override  { this->setCachedModels(models, this->getSimulator()); }
            virtual int updateModels(const CAircraftModelList &models) override  { return m_caches.updateModelsForSimulator(models, this->getSimulator()); }
            virtual void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override  { this->setCachedModels(models, simulator); }
            virtual int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) override  { return m_caches.updateModelsForSimulator(models, simulator); }
            //! @}

            //! Set cached models
            CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator);

        signals:
            //! Simulator has been changed
            void simulatorChanged(const CSimulatorInfo &simulator);

            //! Cache changed
            void cacheChanged(const CSimulatorInfo &simulator);

        protected:
            Data::CModelSetCaches m_caches { true, this }; //!< caches
            CSimulatorInfo m_currentSimulator = CSimulatorInfo::guessDefaultSimulator();

        private:
            //! Change the simulator
            //! \remark unckecked, does not check if simulator is the same
            void changeSimulator(const CSimulatorInfo &simulator);

            //! Model cache has changed
            void onModelsCacheChanged(const CSimulatorInfo &simulator);

            //! Any cached data?
            bool hasCachedData() const;

            //! Cache timestamp
            QDateTime getCacheTimestamp() const;

            //! Clear cache
            BlackMisc::CStatusMessage clearCache();
        };
    } // namespace
} // namespace

#endif // guard
