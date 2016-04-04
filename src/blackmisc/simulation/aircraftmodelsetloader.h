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
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/pixmap.h"
#include <QObject>

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
            CAircraftModelSetLoader(const CSimulatorInfo &info, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftModelSetLoader();

            //! The loaded models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getAircraftModels() const;

            //! Count of loaded models
            int getAircraftModelsCount() const { return getAircraftModels().size(); }

            //! Which simulator is supported by that very loader
            const BlackMisc::Simulation::CSimulatorInfo &getSimulator() const;

            //! Supported simulators as string
            QString getSimulatorAsString() const;

            //! Is the given simulator supported?
            bool supportsSimulator(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Shutdown
            void gracefulShutdown();

        signals:
            //! Simulator has been changed
            void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Cache changed
            void cacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        public slots:
            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            BlackMisc::CStatusMessage setModelsInCache(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Set cache from outside, this should only be used in special cases.
            //! But it allows to modify data elsewhere and update the cache with manipulated data.
            BlackMisc::CStatusMessage replaceOrAddModelsInCache(const CAircraftModelList &models, const CSimulatorInfo &simulator = CSimulatorInfo());

            //! Change the simulator
            void changeSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        protected:
            //! Cache timestamp
            QDateTime getCacheTimestamp() const;

            //! Make sure cache is syncronized
            void syncronizeCache();

            //! Any cached data?
            bool hasCachedData() const;

            //! Clear cache
            BlackMisc::CStatusMessage clearCache();

            BlackMisc::Simulation::CSimulatorInfo        m_simulatorInfo;   //!< Corresponding simulator
            BlackMisc::Simulation::Data::CModelSetCaches m_caches { this }; //!< caches
        };
    } // namespace
} // namespace

#endif // guard
