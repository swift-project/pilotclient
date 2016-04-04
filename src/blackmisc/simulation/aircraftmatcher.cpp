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

        CAircraftModel CAircraftMatcher::getClosestMatch(const CSimulatedAircraft &remoteAircraft, CStatusMessageList *log) const
        {
            const CAircraftModelList matchModels(this->m_modelSet); // Models for this matching
            if (matchModels.isEmpty())
            {
                logDetails(log, remoteAircraft, "No models for matching, using default", CStatusMessage::SeverityWarning);
                return this->getDefaultModel();
            }

            CAircraftModel aircraftModel(remoteAircraft.getModel()); // set defaults

            // Manually set string?
            if (remoteAircraft.getModel().hasManuallySetString())
            {
                // the user did a manual mapping "by hand", so he really should know what he is doing
                // no matching
                logDetails(log, remoteAircraft, "Manually set model " + remoteAircraft.getModelString());
                return remoteAircraft.getModel();
            }

            do
            {
                // try to find in installed models by model string
                aircraftModel = matchByExactModelString(remoteAircraft, matchModels, log);
                if (aircraftModel.hasModelString()) { break; }

                // by livery, then by ICAO
                aircraftModel = matchByLiveryAndIcaoCode(remoteAircraft, matchModels, log);
                if (aircraftModel.hasModelString()) { break; }

                // by ICAO data from set
                aircraftModel = matchModelsByIcaoData(remoteAircraft, matchModels, false, log);
                if (aircraftModel.hasModelString()) { break; }

                // family
                QString family = remoteAircraft.getAircraftIcaoCode().getFamily();
                aircraftModel = matchByFamily(remoteAircraft, family, matchModels, "real family", log);
                if (aircraftModel.hasModelString()) { break; }

                // scenario: the ICAO actually is the family
                family = remoteAircraft.getAircraftIcaoCodeDesignator();
                aircraftModel = matchByFamily(remoteAircraft, family, matchModels, "ICAO treated as family", log);
                if (aircraftModel.hasModelString()) { break; }

                // combined code
                aircraftModel = matchByCombinedCode(remoteAircraft, matchModels, true, log);
                if (aircraftModel.hasModelString()) { break; }

                aircraftModel = getDefaultModel();
                logDetails(log, remoteAircraft, "Using default model " + aircraftModel.getModelString());
            }
            while (false);

            // copy over callsign and other data
            aircraftModel.setCallsign(remoteAircraft.getCallsign());

            Q_ASSERT_X(!aircraftModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign");
            Q_ASSERT_X(aircraftModel.hasModelString(), Q_FUNC_INFO, "Missing model string");
            Q_ASSERT_X(aircraftModel.getModelType() != CAircraftModel::TypeUnknown, Q_FUNC_INFO, "Missing model type");

            return aircraftModel;
        }

        int  CAircraftMatcher::setModelSet(const CAircraftModelList &models)
        {
            CAircraftModelList modelsCleaned(models);
            int r1 = modelsCleaned.removeAllWithoutModelString();
            int r2 = modelsCleaned.removeIfExcluded();
            if ((r1 + r2) > 0)
            {
                CLogMessage(this).warning("Removed models for matcher, without string %1, excluded %2") << r1 << r2;
            }
            if (modelsCleaned.isEmpty())
            {
                CLogMessage(this).error("No models for matching, that will not work");
            }
            else
            {
                CLogMessage(this).info("Set %1 models in matcher") << modelsCleaned.size();
            }
            this->m_modelSet = modelsCleaned;
            return models.size();
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

        CAircraftModel CAircraftMatcher::matchByExactModelString(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &models, CStatusMessageList *log)
        {
            if (remoteAircraft.getModelString().isEmpty())
            {
                if (log) { logDetails(log, remoteAircraft, "No model string, no exact match possible"); }
                return CAircraftModel();
            }

            CAircraftModel model = models.findFirstByModelStringOrDefault(remoteAircraft.getModelString());
            if (log)
            {
                if (model.hasModelString())
                {
                    this->logDetails(log, remoteAircraft, "Found exact match for " + model.getModelString());
                }
                else
                {
                    this->logDetails(log, remoteAircraft, "No exact match for " + model.getModelString());
                }
            }
            model.setModelType(CAircraftModel::TypeModelMatching);
            return model;
        }

        CAircraftModel CAircraftMatcher::matchModelsByIcaoData(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &models, bool ignoreAirline, CStatusMessageList *log) const
        {
            CAircraftModel aircraftModel;
            if (!remoteAircraft.hasAircraftDesignator())
            {
                if (log) { this->logDetails(log, remoteAircraft, "No aircraft designator, skipping step"); }
                return aircraftModel;
            }

            BlackMisc::Simulation::CAircraftModelList mappingModels(
                models.findByIcaoDesignators(remoteAircraft.getAircraftIcaoCode(),
                                             ignoreAirline ? CAirlineIcaoCode() : remoteAircraft.getAirlineIcaoCode()));
            if (!mappingModels.isEmpty())
            {
                aircraftModel = mappingModels.front();
                aircraftModel.setModelType(CAircraftModel::TypeModelMatching);
                if (log)
                {
                    this->logDetails(log, remoteAircraft,
                                     "Possible aircraft " + QString::number(mappingModels.size()) +
                                     ", found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " +
                                     (ignoreAirline ? "" : remoteAircraft.getAirlineIcaoCodeDesignator()));
                }
            }
            else
            {
                if (remoteAircraft.hasAircraftAndAirlineDesignator())
                {
                    // we have searched by aircraft and airline, bout not found anything
                    if (log)
                    {
                        this->logDetails(log, remoteAircraft,
                                         "Not found by ICAO " +
                                         remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator() +
                                         " relaxing to only ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator());
                    }
                    // recursive lookup by ignoring airline
                    return this->matchModelsByIcaoData(remoteAircraft, models, true, log);
                }
                if (log) { this->logDetails(log, remoteAircraft, "Not found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator()); }
            }
            return aircraftModel;
        }

        CAircraftModel CAircraftMatcher::matchByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, const CAircraftModelList &models, const QString &modelSource, CStatusMessageList *log) const
        {
            // Use an algorithm to find the best match
            if (family.isEmpty())
            {
                if (log) { this->logDetails(log, remoteAircraft, "No family, skipping step"); }
                return CAircraftModel();
            }
            if (models.isEmpty())
            {
                if (log) { this->logDetails(log, remoteAircraft, "No models in " + modelSource + " for family match"); }
                return CAircraftModel();
            }

            CAircraftModelList found(models.findByFamily(family));
            if (found.isEmpty())
            {
                if (log) { this->logDetails(log, remoteAircraft, "Not found by family " + family + " in " + modelSource); }
                return CAircraftModel();
            }

            if (remoteAircraft.hasAirlineDesignator())
            {
                const CAircraftModelList foundReduceByAirline = found.findByIcaoDesignators(CAircraftIcaoCode(), remoteAircraft.getAirlineIcaoCode());
                if (!foundReduceByAirline.isEmpty())
                {
                    CAircraftModel model(found.front());
                    if (log)
                    {
                        logDetails(log,
                                   remoteAircraft, "Found by family " + family + " and airline " +
                                   remoteAircraft.getAirlineIcaoCodeDesignator() + " (" + hint + ") as " + model.getAircraftIcaoCodeDesignator() +
                                   " size " + QString::number(foundReduceByAirline.size()));
                    }
                    model.setModelType(CAircraftModel::TypeModelMatching);
                    return model;
                }
            }

            CAircraftModel model(found.front());
            logDetails(log,
                       remoteAircraft, "Found by family " + family + " (" + hint + ") " +
                       model.getAircraftIcaoCodeDesignator() + " size " + QString::number(found.size()));
            model.setModelType(CAircraftModel::TypeModelMatching);
            return model;
        }

        CAircraftModelList CAircraftMatcher::ifPossibleReduceByManufacturer(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, CStatusMessageList *log)
        {
            if (inList.isEmpty())
            {
                if (log) { logDetails(log , remoteAircraft, info + " " + "Empty input list, cannot reduce"); }
                return inList;
            }

            const QString m = remoteAircraft.getAircraftIcaoCode().getManufacturer();
            if (m.isEmpty())
            {
                if (log) { logDetails(log , remoteAircraft, info + " No manufacturer, cannot reduce " + QString::number(inList.size()) +  " entries"); }
                return inList;
            }

            const CAircraftModelList outList(inList.findByManunfacturer(m));
            if (outList.isEmpty())
            {
                if (log) { logDetails(log , remoteAircraft, info + " Not found " + m + ", cannot reduce"); }
                return inList;
            }

            if (log) { logDetails(log , remoteAircraft, info + " Reduced by " + m + " results: " + QString::number(outList.size())); }
            return outList;
        }

        CAircraftModelList CAircraftMatcher::ifPossibleReduceByAirline(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, CStatusMessageList *log)
        {
            if (inList.isEmpty())
            {
                if (log) { logDetails(log , remoteAircraft, info + " " + "Empty input list, cannot reduce"); }
                return inList;
            }

            if (!remoteAircraft.hasAirlineDesignator())
            {
                if (log) { logDetails(log , remoteAircraft, info + " " + "No airline, cannot reduce " + QString::number(inList.size()) +  " entries"); }
                return inList;
            }

            const CAircraftModelList outList(inList.findByIcaoDesignators(CAircraftIcaoCode(), remoteAircraft.getAirlineIcaoCode()));
            if (outList.isEmpty())
            {
                if (log) { logDetails(log , remoteAircraft, info + " Cannot reduce by " + remoteAircraft.getAirlineIcaoCodeDesignator() + " results: " + QString::number(outList.size())); }
            }

            if (log) { logDetails(log , remoteAircraft, info + " Reduced reduce by " + remoteAircraft.getAirlineIcaoCodeDesignator() + " to " + QString::number(outList.size())); }
            return outList;
        }

        void CAircraftMatcher::logDetails(CStatusMessageList *log, const CSimulatedAircraft &remoteAircraft, const QString &message, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            const CStatusMessage m(getLogCategories(), s, remoteAircraft.hasCallsign() ? remoteAircraft.getCallsign().toQString() + ": " + message.trimmed() : message.trimmed());
            log->push_back(m);
        }
    }
} // namespace
