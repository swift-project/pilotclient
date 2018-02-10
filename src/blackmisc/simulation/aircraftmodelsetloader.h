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
            public BlackMisc::Simulation::IModelsSetable,
            public BlackMisc::Simulation::IModelsUpdatable,
            public BlackMisc::Simulation::IModelsPerSimulatorSetable,
            public BlackMisc::Simulation::IModelsPerSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorUpdatable)

        public:
            //! Constructor
            CAircraftModelSetLoader(QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftModelSetLoader();

            //! Make sure cache is synchronized
            bool synchronizeCache();

            //! Admit current cache
            bool admitCache();

            //! The loaded models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getAircraftModels() const;

            //! The loaded models for given simulator
            //! \threadsafe
            //! \remark non-const because it synchronizes cache
            BlackMisc::Simulation::CAircraftModelList getAircraftModels(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Count of loaded models
            //! \threadsafe
            int getAircraftModelsCount() const;

            //! Model for given model string
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

            //! Models from cache
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

            //! Current simulator
            CSimulatorInfo getSimulator() const;

            //! Supported simulators as string
            QString getSimulatorAsString() const;

            //! Set simulator
            //! \remark checked version, does nothing if simulator is alread set
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Is the given simulator supported?
            bool supportsSimulator(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Simulators with initialized caches
            CSimulatorInfo simulatorsWithInitializedModelSet() const;

            //! Shutdown
            void gracefulShutdown();

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoString
            QString getInfoString() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoStringFsFamily
            QString getInfoStringFsFamily() const;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setCachedModels(models, this->getSimulator()); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->replaceOrAddCachedModels(models, this->getSimulator()); }
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->setCachedModels(models, simulator); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->replaceOrAddCachedModels(models, simulator); }
            //! @}

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            BlackMisc::CStatusMessage setCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            BlackMisc::CStatusMessage replaceOrAddCachedModels(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

        signals:
            //! Simulator has been changed
            void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Cache changed
            void cacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        protected:
            BlackMisc::Simulation::Data::CModelSetCaches m_caches { true, this }; //!< caches

        private:
            //! Change the simulator
            //! \remark unckecked, does not check if simulator is the same
            void changeSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Model cache has changed
            void onModelsCacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

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
