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
                ByModelString    = 1 << 0,
                ByIcaoData       = 1 << 1,
                ByFamily         = 1 << 2,
                All              = ByModelString | ByIcaoData | ByFamily
            };
            Q_DECLARE_FLAGS(MatchingMode, MatchingModeFlag)

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            CAircraftMatcher(MatchingMode matchingMode = All, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftMatcher();

            //! Set the enabled matching modes
            void setMatchingModes(MatchingMode matchingModes);

            //! Get the closest matching aircraft model.
            //! Result depends on enabled modes.
            //! \sa MatchingModeFlag
            //! \threadsafe
            CAircraftModel getClosestMatch(const CSimulatedAircraft &remoteAircraft, BlackMisc::CStatusMessageList *log = nullptr) const;

            //! Get the models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getModels() const { return m_models.read(); }

            //! Set the models we want to use
            //! \threadsafe
            int setModels(const BlackMisc::Simulation::CAircraftModelList &models);

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
            CAircraftModel matchByExactModelString(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &models, CStatusMessageList *log) const;

            //! Installed models by ICAO data
            //! \threadsafe
            CAircraftModel matchModelsByIcaoData(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &models, bool ignoreAirline, CStatusMessageList *log) const;

            //! Find model by aircraft family
            CAircraftModel matchByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, const CAircraftModelList &models, CStatusMessageList *log) const;

            //! Add to log. if applicable
            void logDetails(BlackMisc::CStatusMessageList *log,
                            const CSimulatedAircraft &remoteAircraft,
                            const QString &message,
                            CStatusMessage::StatusSeverity s = CStatusMessage::SeverityInfo) const;

            MatchingMode                           m_matchingMode = All;
            BlackMisc::Simulation::CAircraftModel  m_defaultModel;                             //!< model to be used as default model
            BlackMisc::LockFree<BlackMisc::Simulation::CAircraftModelList> m_models;           //!< models used for model matching
        };
    }
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftMatcher::MatchingMode)

#endif // guard
