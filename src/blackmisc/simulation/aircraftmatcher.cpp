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
        const CLogCategoryList &CAircraftMatcher::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::matching() };
            return cats;
        }

        CAircraftMatcher::CAircraftMatcher(MatchingMode matchingMode, QObject *parent) :
            QObject(parent),
            m_matchingMode(matchingMode)
        { }

        CAircraftMatcher::~CAircraftMatcher()
        { }

        void CAircraftMatcher::setModelMappingProvider(std::unique_ptr<IModelMappingsProvider> mappings)
        {
            this->m_mappingsProvider = std::move(mappings);
            CAircraftModelList models = this->m_mappingsProvider->getMatchingModels();
            int d = models.removeIfExcluded();
            this->m_models.uniqueWrite() = models; // local copy
            CLogMessage(this).info("Added %1 models for model matching, %2 removed") << models.size() << d;
        }

        void CAircraftMatcher::setLogDetails(bool log)
        {
            this->m_logDetails = log;
        }

        void CAircraftMatcher::reload()
        {

        }

        CAircraftModel CAircraftMatcher::getClosestMatch(const CSimulatedAircraft &remoteAircraft) const
        {
            CAircraftModel aircraftModel(remoteAircraft.getModel()); // set defaults

            // Manually set string?
            if (remoteAircraft.getModel().hasManuallySetString())
            {
                // the user did a manual mapping "by hand", so he really should know what he is doing
                // no matching
                this->logDetails(remoteAircraft, "Manually set model " + remoteAircraft.getModelString());
                return remoteAircraft.getModel();
            }

            QString log;
            do
            {
                // try to find in installed models by model string
                aircraftModel = matchByExactModelString(remoteAircraft);
                if (aircraftModel.hasModelString())
                {
                    if (this->m_logDetails) { log = "Matched by exact model string " + aircraftModel.getModelString(); }
                    break;
                }

                // ------------ start parts depending on swift DB data -------------------

                // by DB ICAO data
                aircraftModel = matchModelsByIcaoData(remoteAircraft, log);
                if (aircraftModel.hasModelString())
                {
                    break;
                }

                // ------------ end parts depending on swift DB data -------------------

                aircraftModel = matchByFamily(remoteAircraft, log);
                if (aircraftModel.hasModelString()) { break; }

                aircraftModel = getDefaultModel();
            }
            while (false);

            // copy over callsign and other data
            aircraftModel.setCallsign(remoteAircraft.getCallsign());

            this->logDetails(aircraftModel, log);

            Q_ASSERT_X(!aircraftModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign");
            Q_ASSERT_X(aircraftModel.hasModelString(), Q_FUNC_INFO, "Missing model string");
            Q_ASSERT_X(aircraftModel.getModelType() != CAircraftModel::TypeUnknown, Q_FUNC_INFO, "Missing model type");

            return aircraftModel;
        }

        const CAircraftModel &CAircraftMatcher::getDefaultModel() const
        {
            return m_defaultModel;
        }

        void CAircraftMatcher::setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel)
        {
            m_defaultModel = defaultModel;
            m_defaultModel.setModelType(CAircraftModel::TypeModelMatchingDefaultModel);
        }

        CAircraftModel CAircraftMatcher::matchByExactModelString(const CSimulatedAircraft &remoteAircraft) const
        {
            const CAircraftModelList models(this->m_models.read());
            return models.findFirstByModelStringOrDefault(remoteAircraft.getModelString());
        }

        CAircraftModel CAircraftMatcher::matchModelsByIcaoData(const CSimulatedAircraft &remoteAircraft, QString &log) const
        {
            CAircraftModel aircraftModel;
            const CAircraftModelList models(this->m_models.read());
            BlackMisc::Simulation::CAircraftModelList mappingModels(
                models.findByIcaoDesignators(remoteAircraft.getAircraftIcaoCode(), remoteAircraft.getAirlineIcaoCode())
            );
            if (!mappingModels.isEmpty())
            {
                log = "Found by ICAO " + remoteAircraft.getAircraftIcaoCode().getDesignator() + " " + remoteAircraft.getAircraftIcaoCode().getDesignator();
                log = log.trimmed();
                CAircraftModel aircraftModel(mappingModels.front());
                aircraftModel.setModelType(CAircraftModel::TypeModelMatching);
            }
            return aircraftModel;
        }

        CAircraftModel CAircraftMatcher::matchByFamily(const CSimulatedAircraft &remoteAircraft, QString &log) const
        {
            // Use an algorithm to find the best match
            Q_UNUSED(remoteAircraft);
            Q_UNUSED(log);
            return CAircraftModel();
        }

        void CAircraftMatcher::logDetails(const CSimulatedAircraft &remoteAircraft, const QString &message) const
        {
            if (!this->m_logDetails || message.isEmpty()) { return; }
            const CCallsign callsign(remoteAircraft.getCallsign());
            CLogMessage(this).info(callsign.toQString(true) + ": " + message);
        }
    }
} // namespace
