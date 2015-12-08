/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmatcher.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"
#include <utility>
#include <atomic>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {

        CAircraftMatcher::CAircraftMatcher(MatchingMode matchingMode, QObject *parent) :
            QObject(parent),
            m_matchingMode(matchingMode)
        { }

        CAircraftMatcher::~CAircraftMatcher()
        {
            cancelInit();
            if (this->m_initWorker) { this->m_initWorker->waitForFinished(); }
        }

        void CAircraftMatcher::init()
        {
            if (m_initState != NotInitialized) { return; }
            m_initWorker = BlackMisc::CWorker::fromTask(this, "CAircraftMatcher::initImpl", [this]()
            {
                this->initImpl();
            });
        }

        bool CAircraftMatcher::isInitialized() const
        {
            return m_initState == InitFinished;
        }

        void CAircraftMatcher::setModelMappingProvider(std::unique_ptr<IModelMappingsProvider> mappings)
        {
            m_mappingsProvider = std::move(mappings);
            if (m_matchingMode.testFlag(ModelMapping)) { initMappings(); }
        }

        void CAircraftMatcher::setMatchingModes(MatchingMode matchingModes)
        {
            m_matchingMode = matchingModes;
            if (m_matchingMode.testFlag(ModelMapping) && m_modelsFromDatastoreInstalled.isEmpty())
            {
                initMappings();
            }
        }

        CAircraftModel CAircraftMatcher::getClosestMatch(const CSimulatedAircraft &remoteAircraft)
        {
            CAircraftModel aircraftModel(remoteAircraft.getModel()); // set defaults

            // Manually set string?
            if (remoteAircraft.getModel().hasManuallySetString())
            {
                // manual set model, maybe update missing parts
                aircraftModel.updateMissingParts(remoteAircraft.getModel());
                aircraftModel.setCallsign(remoteAircraft.getCallsign());
                return aircraftModel;
            }

            // mapper ready?
            if (!isInitialized())
            {
                // will be removed later, just for experimental version
                aircraftModel = getDefaultModel();
                aircraftModel.setCallsign(remoteAircraft.getCallsign());
                CLogMessage(static_cast<CAircraftMatcher *>(nullptr)).warning("Matcher not initialized, set to default model");
                return aircraftModel;
            }

            do
            {
                //! \todo this code here is partially nuts and needs to be adjusted
                //!       as soon as we have real DB data
                //! things to change:
                //! 1) instead of keeping two lists (DB data + own models) just
                //!    update / enhance the list of installed models with the DB metadata
                //!    then there is only one search required
                //! 2) Keep a cache of installed models enriched by the DB metadata (that will eliminate all
                //!    sync processes and can be done offline
                //! 3) drivers which can assign reliable ICAO codes should do this as much as possible
                //!    the sync process with DB will improve those information (by livery details, but in case
                //!    of now metadata from DB we have the best matching data we can get
                //!    Also such data can be used in the mapping tool as default values (Less values to type in)

                // try to find in installed models by model string
                aircraftModel = matchByExactModelName(remoteAircraft);
                if (aircraftModel.hasModelString()) { break; }

                // ------------ start parts depending on swift DB data -------------------

                // by DB ICAO data
                aircraftModel = matchInstalledModelsByIcaoData(remoteAircraft);
                if (aircraftModel.hasModelString()) { break; }

                // ------------ end parts depending on swift DB data -------------------

                aircraftModel = matchByAlgorithm(remoteAircraft);
                if (aircraftModel.hasModelString()) { break; }

                aircraftModel = getDefaultModel();
            }
            while (false);

            // copy over callsign
            aircraftModel.setCallsign(remoteAircraft.getCallsign());

            Q_ASSERT_X(!aircraftModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign");
            Q_ASSERT_X(aircraftModel.hasModelString(), Q_FUNC_INFO, "Missing model string");
            Q_ASSERT_X(aircraftModel.getModelType() != CAircraftModel::TypeUnknown, Q_FUNC_INFO, "Missing model type");

            return aircraftModel;
        }

        int CAircraftMatcher::synchronize()
        {
            return synchronizeWithExistingModels(m_installedModels.getSortedModelStrings());
        }

        void CAircraftMatcher::cancelInit()
        {
            // when running, force re-init
            this->m_initState = NotInitialized;
        }

        const CAircraftModel &CAircraftMatcher::getDefaultModel()
        {
            return m_defaultModel;
        }

        void CAircraftMatcher::setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel)
        {
            m_defaultModel = defaultModel;
            m_defaultModel.setModelType(CAircraftModel::TypeModelMatchingDefaultModel);
        }

        void CAircraftMatcher::ps_setDatastoreModels(const CAircraftModelList &mappings)
        {
            m_modelsFromDatastoreInstalled = mappings;
        }

        void CAircraftMatcher::initImpl()
        {
            InitState e = NotInitialized;
            InitState d = InitInProgress;
            if (!m_initState.compare_exchange_strong(e, d)) { return; }

            // sync
            this->synchronize();
            CLogMessage(this).debug() << "Mapping definitions after sync" << m_modelsFromDatastoreInstalled.size();

            // finish
            CLogMessage(this).info("Mapping system: %1 definitions for %2 installed models") << m_modelsFromDatastoreInstalled.size()
                    << m_installedModels.size();
            m_initState = InitFinished;
            emit initializationFinished();
        }

        void CAircraftMatcher::initMappings()
        {
            Q_ASSERT(m_mappingsProvider);
            int mappingsSize = m_mappingsProvider->getDatastoreModels().size();
            if (mappingsSize < 1)
            {
                m_mappingsProvider->read();
                m_modelsFromDatastoreInstalled = m_mappingsProvider->getDatastoreModels();
                mappingsSize = m_modelsFromDatastoreInstalled.size();
                if (mappingsSize < 1)
                {
                    CLogMessage(this).error("Reading mapping rules failed or empty!");
                    // Turn off the model mapping mode
                    m_matchingMode &= ~ModelMapping;
                    return;
                }
            }
            m_modelsFromDatastoreInstalled = m_mappingsProvider->getDatastoreModels();
            CLogMessage(this).debug() << "Mapping definitions" << mappingsSize;
        }

        CAircraftModel CAircraftMatcher::matchByExactModelName(const CSimulatedAircraft &remoteAircraft)
        {
            return this->m_installedModels.findFirstByModelString(remoteAircraft.getModelString());
        }

        CAircraftModel CAircraftMatcher::matchInstalledModelsByIcaoData(const CSimulatedAircraft &remoteAircraft)
        {
            CAircraftModel aircraftModel;
            if (m_modelsFromDatastoreInstalled.isEmpty()) { return aircraftModel; }
            BlackMisc::Simulation::CAircraftModelList datastoreModels(m_modelsFromDatastoreInstalled.findByIcaoDesignators(remoteAircraft.getAircraftIcaoCode(), remoteAircraft.getAirlineIcaoCode()));
            if (!datastoreModels.isEmpty())
            {
                CAircraftModel aircraftModel(datastoreModels.front());
                aircraftModel.setModelType(CAircraftModel::TypeModelMatching);
            }
            return aircraftModel;
        }

        CAircraftModel CAircraftMatcher::matchByAlgorithm(const CSimulatedAircraft & /** remoteAircraft **/)
        {
            // Use an algorithm to find the best match
            return CAircraftModel();
        }

        int CAircraftMatcher::synchronizeWithExistingModels(const QStringList &modelNames, Qt::CaseSensitivity cs)
        {
            if (modelNames.isEmpty() || m_modelsFromDatastoreInstalled.isEmpty()) { return 0; }
            CAircraftModelList newList;
            for (const CAircraftModel &modelDatastore : m_modelsFromDatastoreInstalled)
            {
                if (this->m_initState != InitInProgress) { return 0; } // canceled
                QString modelString(modelDatastore.getModelString());
                if (modelString.isEmpty()) { continue; }
                if (modelNames.contains(modelString, cs))
                {
                    newList.push_back(modelDatastore);
                }
            }
            this->m_modelsFromDatastoreInstalled = newList;
            return this->m_modelsFromDatastoreInstalled.size();
        }
    }
} // namespace
