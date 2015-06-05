/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTMATCHER_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTMATCHER_H

#include "aircraftcfgentrieslist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/modelmappingsprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/worker.h"
#include <QObject>
#include <QScopedPointer>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QReadWriteLock>
#include <memory>
#include <atomic>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Mapper for all models (works for FS9/FSX).
            //! \details Reads all the mapping rules and all the available flight simulator models.
            //!          Then all rules for models not existing are eliminated ( \sa synchronize ).
            //!          Thereafter all existing models and mappings can be obtained from here.
            //! \sa CAircraftCfgEntries
            //! \sa CAircraftCfgEntriesList
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
                    ExactMatch = 1 << 0,
                    ModelMapping = 1 << 1,
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
                const BlackMisc::Network::CAircraftMappingList &getAircraftMappingList() const { return m_mappingsProvider->getMappingList(); }

                //! Inverse lookup
                BlackMisc::Aviation::CAircraftIcaoData getIcaoForModelString(const QString &modelString) const;

                //! Number of mapping definitions
                int countMappingRules() const { return m_modelMappings.size(); }

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
                //! Set the mapping rules
                void ps_setModelMappingRules(const BlackMisc::Network::CAircraftMappingList &mappings);

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

                CAircraftModel matchByExactModelName(const CSimulatedAircraft &remoteAircraft);
                CAircraftModel matchByMapping(const CSimulatedAircraft &remoteAircraft);
                CAircraftModel matchByAlgorithm(const CSimulatedAircraft &remoteAircraft);

                //! Synchronize with existing model names, remove unneeded models
                int synchronizeWithExistingModels(const QStringList &modelNames, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

                //! Reverse lookup
                void reverseLookupIcaoData(BlackMisc::Simulation::CAircraftModel &model);

                std::unique_ptr<BlackMisc::Simulation::IModelMappingsProvider> m_mappingsProvider; //!< Provides all mapping definitions
                std::atomic<InitState> m_initState { NotInitialized };
                QPointer<BlackMisc::CWorker> m_initWorker;
                MatchingMode m_matchingMode = ModelMatching;
                CAircraftModelList m_installedModels;
                BlackMisc::Network::CAircraftMappingList m_modelMappings;
                BlackMisc::Simulation::CAircraftModel m_defaultModel;
            };
        } // namespace
    } // namespace
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::FsCommon::CAircraftMatcher::MatchingMode)

#endif // guard
