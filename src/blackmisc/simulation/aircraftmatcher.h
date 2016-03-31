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
#include <QObject>
#include <QPointer>
#include <QThread>
#include <memory>
#include <atomic>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Matcher for all models.
         * \details Reads all the mapping rules and all the available flight simulator models.
         *          Then all rules for models not existing are eliminated ( \sa synchronize ).
         *          Thereafter all existing models and mappings can be obtained from here.
         */
        class BLACKMISC_EXPORT CAircraftMatcher : public QObject
        {
            Q_OBJECT

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

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            CAircraftMatcher(MatchingMode matchingMode = ModelMatching, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftMatcher();

            //! Set the enabled matching modes
            void setMatchingModes(MatchingMode matchingModes);

            //! Get the closest matching aircraft model.
            //! Result depends on enabled modes.
            //! \sa MatchingModeFlag
            //! \threadsafe
            CAircraftModel getClosestMatch(const CSimulatedAircraft &remoteAircraft) const;

            //! Get all mappings
            BlackMisc::Simulation::CAircraftModelList getMatchingModels() const { return m_mappingsProvider->getMatchingModels(); }

            //! Set the model mapping provider. The CAircraftMatcher will move the object and take over ownership
            void setModelMappingProvider(std::unique_ptr<IModelMappingsProvider> mappings);

            //! Log.details?
            void setLogDetails(bool log);

            //! Reload
            void reload();

            //! Default model
            const BlackMisc::Simulation::CAircraftModel &getDefaultModel() const;

            //! Set default model
            void setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel);

        private:
            //! Init state
            enum InitState
            {
                NotInitialized,
                InitInProgress,
                InitFinished
            };

            //! Search in models by key (aka model string)
            //! \threadsafe
            CAircraftModel matchByExactModelString(const CSimulatedAircraft &remoteAircraft) const;

            //! Installed models by ICAO data
            //! \threadsafe
            CAircraftModel matchModelsByIcaoData(const CSimulatedAircraft &remoteAircraft, QString &log) const;

            //! Find model by aircraft family
            CAircraftModel matchByFamily(const CSimulatedAircraft &remoteAircraft, QString &log) const;

            //! Log. details about mapping of particular aircraft
            //! threadsafe
            void logDetails(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const QString &message) const;

            MatchingMode                           m_matchingMode = ModelMatching;
            BlackMisc::Simulation::CAircraftModel  m_defaultModel;                             //!< model to be used as default model
            BlackMisc::LockFree<BlackMisc::Simulation::CAircraftModelList> m_models;           //!< models used for model matching
            std::unique_ptr<BlackMisc::Simulation::IModelMappingsProvider> m_mappingsProvider; //!< Provides all mapping definitions
            std::atomic<bool>                      m_logDetails { false };                     //!< log details
        };
    }
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftMatcher::MatchingMode)

#endif // guard
