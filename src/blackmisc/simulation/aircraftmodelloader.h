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
#include "blackmisc/pixmap.h"
#include <QObject>
#include <atomic>

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
            enum LoadMode
            {
                ModeBlocking,
                ModeBackground
            };

            //! Destructor
            virtual ~IAircraftModelLoader();

            //! Start the loading process
            virtual void startLoading(LoadMode mode = ModeBackground) = 0;

            //! Loading finished?
            virtual bool isLoadingFinished() const = 0;

            //! The models loaded
            virtual BlackMisc::Simulation::CAircraftModelList getAircraftModels() const = 0;

            //! A representive pixmap for given model
            virtual BlackMisc::CPixmap iconForModel(const QString &modelName, BlackMisc::CStatusMessage &statusMessage) const = 0;

            //! Which simulators are supported by that very loader
            const BlackMisc::Simulation::CSimulatorInfo &supportedSimulators() const;

            //! Supported simulators as string
            QString supportedSimulatorsAsString() const;

            //! Is the given simulator supported?
            bool supportsSimulator(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Cancel read
            void cancelLoading();

            //! Shutdown
            void gracefulShutdown();

            //! Create a loader
            static std::unique_ptr<IAircraftModelLoader> createModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

        signals:
            //! Parsing is finished
            void loadingFinished(bool success);

        protected:
            //! Constructor
            IAircraftModelLoader(const CSimulatorInfo &info = CSimulatorInfo());

            BlackMisc::Simulation::CSimulatorInfo m_simulatorInfo; //!< Corresponding simulator
            std::atomic<bool> m_cancelLoading { false }; //!< flag
        };

    } // namespace
} // namespace

#endif // guard
