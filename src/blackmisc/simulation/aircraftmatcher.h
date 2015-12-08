/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMATCHER_H
#define BLACKMISC_SIMULATION_AIRCRAFTMATCHER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/modelmappingsprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/worker.h"
#include <QObject>
#include <QPointer>
#include <QThread>
#include <memory>
#include <atomic>

namespace BlackMisc
{
    namespace Simulation
    {
        /*! Matcher for all models
         * \details Reads all the mapping rules and all the available flight simulator models.
         *          Then all rules for models not existing are eliminated ( \sa synchronize ).
         *          Thereafter all existing models and mappings can be obtained from here.
         */
        class BLACKMISC_EXPORT CAircraftMatcher : public QObject
        {
            Q_OBJECT

        signals:
            //! Full init completed
            void initializationFinished();

        public:
            //! Enabled matching mode flags
            enum MatchingModeFlag
            {
                ExactMatch    = 1 << 0,
                ModelMapping  = 1 << 1,
                ModelMatching = 1 << 2,
                AllModes = ExactMatch | ModelMapping | ModelMatching
            };
            Q_DECLARE_FLAGS(MatchingMode, MatchingModeFlag)

            //! Constructor
            CAircraftMatcher(MatchingMode matchingMode = ModelMatching, QObject *parent = nullptr);

            //! Destructor
            ~CAircraftMatcher();

            //! Initialize
            void init();

            //! Init completed?
            bool isInitialized() const;

            //! Get all models
            const CAircraftModelList &getInstalledModelsList() const { return m_installedModels; }

            //! Set the list of installed models
            void setInstalledModels(const CAircraftModelList &models) { m_installedModels = models; }

            //! Number of models
            int countInstalledModels() const { return m_installedModels.size(); }

            //! Set the model mapping provider. The CAircraftMatcher will move the object and take over ownership
            void setModelMappingProvider(std::unique_ptr<IModelMappingsProvider> mappings);

            //! Set the enabled matching modes
            void setMatchingModes(MatchingMode matchingModes);

            //! Get the closest matching aircraft model.
            //! Result depends on enabled modes.
            //! \sa MatchingModeFlag
            CAircraftModel getClosestMatch(const CSimulatedAircraft &remoteAircraft);

            //! Get all mappings
            const BlackMisc::Simulation::CAircraftModelList &getDatastoreModels() const { return m_mappingsProvider->getDatastoreModels(); }

            //! Number of mapping definitions
            int countMappingRules() const { return m_modelsFromDatastoreInstalled.size(); }

            //! Synchronize models and mappings
            //! \remarks after this step, we only have mappings for which we have models
            int synchronize();

            //! Shutdown
            void cancelInit();

            //! default model
            const BlackMisc::Simulation::CAircraftModel &getDefaultModel();

            //! Set default model
            void setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel);

        private slots:
            //! Set the datatstore models
            void ps_setDatastoreModels(const CAircraftModelList &mappings);

        private:
            //! Init state
            enum InitState
            {
                NotInitialized,
                InitInProgress,
                InitFinished
            };

            void initImpl();
            void initMappings();

            //! Search in installed models by key (aka model string)
            CAircraftModel matchByExactModelName(const CSimulatedAircraft &remoteAircraft);

            //! Installed models by database ICAO data (requires DB entries)
            CAircraftModel matchInstalledModelsByIcaoData(const CSimulatedAircraft &remoteAircraft);

            CAircraftModel matchByAlgorithm(const CSimulatedAircraft &remoteAircraft);

            //! Synchronize with existing model names, remove unneeded models
            int synchronizeWithExistingModels(const QStringList &modelNames, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

            std::unique_ptr<BlackMisc::Simulation::IModelMappingsProvider> m_mappingsProvider; //!< Provides all mapping definitions
            std::atomic<InitState> m_initState { NotInitialized };
            QPointer<BlackMisc::CWorker> m_initWorker;
            MatchingMode m_matchingMode = ModelMatching;
            CAircraftModelList m_installedModels;                 //!< my simulator`s installed models
            CAircraftModelList m_modelsFromDatastoreInstalled;    //!< models from datastore I do actually have installed
            BlackMisc::Simulation::CAircraftModel m_defaultModel; //!< model to be used as default model
        };
    }
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftMatcher::MatchingMode)

#endif // guard
