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
#include "blackcore/application.h"
#include <utility>
#include <atomic>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackCore
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
        CAircraftModelList matchedModels(this->m_modelSet); // Models for this matching
        const MatchingMode mode = this->m_matchingMode;

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
            if (mode.testFlag(ByModelString))
            {
                aircraftModel = matchByExactModelString(remoteAircraft, matchModels, log);
                if (aircraftModel.hasModelString()) { break; }
            }
            else if (log)
            {
                logDetails(log, remoteAircraft, "Skipping model string match");
            }

            // by livery, then by ICAO
            if (mode.testFlag(ByLivery))
            {
                aircraftModel = matchByLiveryAndIcaoCode(remoteAircraft, matchModels, log);
                if (aircraftModel.hasModelString()) { break; }
            }
            else if (log)
            {
                logDetails(log, remoteAircraft, "Skipping livery match");
            }

            // by ICAO data from set
            aircraftModel = matchModelsByIcaoData(remoteAircraft, matchModels, false, log);
            if (aircraftModel.hasModelString()) { break; }

            // family
            if (mode.testFlag(ByFamily))
            {
                QString family = remoteAircraft.getAircraftIcaoCode().getFamily();
                aircraftModel = matchByFamily(remoteAircraft, family, matchModels, "real family", log);
                if (aircraftModel.hasModelString()) { break; }

                // scenario: the ICAO actually is the family
                family = remoteAircraft.getAircraftIcaoCodeDesignator();
                aircraftModel = matchByFamily(remoteAircraft, family, matchModels, "ICAO treated as family", log);
                if (aircraftModel.hasModelString()) { break; }
            }
            else if (log)
            {
                logDetails(log, remoteAircraft, "Skipping family match");
            }

            // combined code
            if (mode.testFlag(ByCombinedCode))
            {
                aircraftModel = matchByCombinedCode(remoteAircraft, matchModels, true, log);
                if (aircraftModel.hasModelString()) { break; }
            }
            else if (log)
            {
                logDetails(log, remoteAircraft, "Skipping combined code match");
            }

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

    CAircraftModel CAircraftMatcher::reverseLookup(const CAircraftModel &modelToLookup, const QString &liveryInfo, CStatusMessageList *log)
    {
        CAircraftModel model(modelToLookup);
        const CCallsign callsign(model.getCallsign());
        if (model.hasModelString())
        {
            // if we find the model here we have a fully defined DB model
            const CAircraftModel modelFromDb(sApp->getWebDataServices()->getModelForModelString(model.getModelString()));
            if (modelFromDb.hasValidDbKey())
            {
                model = modelFromDb;
                if (log) { logDetails(log, callsign, QString("Reverse looked up DB model `%1` for %2").arg(modelFromDb.getDbKey()).arg(callsign.toQString())); }
            }
        }

        // only if not yet matched with DB
        const QString aircraftIcaoDesignator(model.getAircraftIcaoCodeDesignator());
        const QString airlineIcaoDesignator(model.getAirlineIcaoCodeDesignator());
        if (!model.hasValidDbKey())
        {
            // try to match by livery
            if (liveryCode.isEmpty() && !airlineIcaoDesignator.isEmpty())
            {
                // we create a standard livery code, then we try to find based on this
                liveryCode = CLivery::getStandardCode(model.getAirlineIcaoCode());
            }

            if (CLivery::isValidCombinedCode(liveryCode))
            {
                // search DB model by livery
                const CAircraftModelList models(sApp->getWebDataServices()->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftIcaoDesignator, liveryCode));
                if (models.isEmpty())
                {
                    // no models for that livery, search for livery only
                    const CLivery databaseLivery(sApp->getWebDataServices()->getLiveryForCombinedCode(liveryCode));
                    if (databaseLivery.hasValidDbKey())
                    {
                        // we have found a livery in the DB
                        model.setLivery(databaseLivery);
                        if (log) { logDetails(log, callsign, QString("Reverse lookup, set livery `%1`").arg(databaseLivery.getCombinedCodePlusInfo())); }
                    }
                }
                else
                {
                    // model by livery data found
                    model = models.front();
                    if (log) { logDetails(log, callsign, QString("Reverse lookup, DB model `%1` for %2/%3, found %4").arg(model.getDbKey()).arg(aircraftIcaoDesignator).arg(liveryCode).arg(models.size())); }
                }
            }

            // if no DB livery, create own dummy livery
            if (!model.hasValidDbKey() && !model.getLivery().hasValidDbKey())
            {
                // create a pseudo livery, try to find airline first
                CAirlineIcaoCode airlineIcao(sApp->getWebDataServices()->smartAirlineIcaoSelector(CAirlineIcaoCode(airlineIcaoDesignator)));
                if (!airlineIcao.hasValidDbKey())
                {
                    // no DB data, we update as much as possible
                    airlineIcao = model.getAirlineIcaoCode();
                    airlineIcao.updateMissingParts(CAirlineIcaoCode(airlineIcaoDesignator));
                }
                if (!airlineIcaoDesignator.isEmpty())
                {
                    const CLivery liveryDummy(CLivery::getStandardCode(airlineIcao), airlineIcao, "Generated");
                    model.setLivery(liveryDummy);
                    if (log) { logDetails(log, callsign, QString("Reverse lookup, set dummy livery `%1`").arg(liveryDummy.getCombinedCodePlusInfo())); }
                }
            }

            if (!model.getAircraftIcaoCode().hasValidDbKey())
            {
                CAircraftIcaoCode aircraftIcao(sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(aircraftIcaoDesignator));
                if (!aircraftIcao.hasValidDbKey())
                {
                    // no DB data, we update as much as possible
                    aircraftIcao = model.getAircraftIcaoCode();
                    aircraftIcao.updateMissingParts(CAircraftIcaoCode(aircraftIcaoDesignator));
                    if (log) { logDetails(log, callsign, QString("Reverse lookup, aircraft `%1` not found in DB").arg(aircraftIcaoDesignator)); }
                }
                model.setAircraftIcaoCode(aircraftIcao);
                if (log) { logDetails(log, callsign, QString("Reverse lookup, set aircraft `%1`").arg(aircraftIcao.getCombinedIcaoStringWithKey())); }
            }
        } // model from DB

        if (model.getModelType() != CAircraftModel::TypeUnknown) { model.setModelType(modelToLookup.getModelType()); }
        model.setCallsign(modelToLookup.getCallsign());
        return model;
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
                logDetails(log, remoteAircraft, "Found exact match for " + model.getModelString());
            }
            else
            {
                logDetails(log, remoteAircraft, "No exact match for " + model.getModelString());
            }
        }
        model.setModelType(CAircraftModel::TypeModelMatching);
        model.setCallsign(remoteAircraft.getCallsign());
        return model;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByLiveryAndIcaoCode(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getLivery().hasCombinedCode())
        {
            if (log) { logDetails(log, remoteAircraft, "No livery code, no reduction possible"); }
            return inList;
        }

        const CAircraftModelList byLivery(
            inList.findByAircraftDesignatorAndLiveryCombinedCode(
                remoteAircraft.getLivery().getCombinedCode(),
                remoteAircraft.getAircraftIcaoCodeDesignator()
            ));

        if (byLivery.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "Not found by livery code " + remoteAircraft.getLivery().getCombinedCode()); }
            return inList;
        }
        reduced = true;
        return byLivery;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByIcaoData(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool ignoreAirline, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.hasAircraftDesignator())
        {
            if (log) { logDetails(log, remoteAircraft, "No aircraft designator, skipping step"); }
            return inList;
        }

        BlackMisc::Simulation::CAircraftModelList searchModels(inList.findByIcaoDesignators(
                    remoteAircraft.getAircraftIcaoCode(),
                    ignoreAirline ? CAirlineIcaoCode() : remoteAircraft.getAirlineIcaoCode()));

        if (!searchModels.isEmpty())
        {
            if (log)
            {
                logDetails(log, remoteAircraft,
                           "Possible aircraft " + QString::number(searchModels.size()) +
                           ", found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " +
                           (ignoreAirline ? "" : remoteAircraft.getAirlineIcaoCodeDesignator()));
            }
            reduced = true;
            return searchModels;
        }
        else
        {
            if (!ignoreAirline && remoteAircraft.hasAircraftAndAirlineDesignator())
            {
                // we have searched by aircraft and airline, but not found anything
                if (log)
                {
                    logDetails(log, remoteAircraft,
                               "Not found by ICAO " +
                               remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator() +
                               " relaxing to only ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator());
                }
                // recursive lookup by ignoring airline
                return ifPossibleReduceByIcaoData(remoteAircraft, inList, true, reduced, log);
            }
            if (log) { logDetails(log, remoteAircraft, "Not found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator()); }
        }
        return inList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, const CAircraftModelList &inList, const QString &hint, bool &reduced, CStatusMessageList *log)
    {
        // Use an algorithm to find the best match
        reduced = true;
        if (family.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "No family, skipping step (" + hint + ")"); }
            return inList;
        }
        if (inList.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "No models for family match (" + hint + ")"); }
            return inList;
        }

        CAircraftModelList found(inList.findByFamily(family));
        if (found.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "Not found by family " + family + " (" + hint + ")"); }
            return inList;
        }

        reduced = true;
        logDetails(log, remoteAircraft, "Found by family " + family + " (" + hint + ") size " + QString::number(found.size()));
        return found;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByManufacturer(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
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
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAirline(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
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
            return inList;
        }

        if (log) { logDetails(log , remoteAircraft, info + " Reduced reduce by " + remoteAircraft.getAirlineIcaoCodeDesignator() + " to " + QString::number(outList.size())); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByCombinedCode(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool relaxIfNotFound, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getAircraftIcaoCode().hasValidCombinedType())
        {
            if (log) { logDetails(log, remoteAircraft, "No valid combined code"); }
            return inList;
        }

        const QString cc = remoteAircraft.getAircraftIcaoCode().getCombinedType();
        CAircraftModelList byCombinedCode(inList.findByCombinedCode(cc));
        if (byCombinedCode.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "Not found by combined code " + cc); }
            if (relaxIfNotFound)
            {

            }
            return inList;
        }

        if (log) { logDetails(log, remoteAircraft, "Found by combined code " + cc + ", possible " + QString::number(byCombinedCode.size())); }
        if (byCombinedCode.size() > 1)
        {
            byCombinedCode = ifPossibleReduceByAirline(remoteAircraft, byCombinedCode, "Combined code", reduced, log);
            byCombinedCode = ifPossibleReduceByManufacturer(remoteAircraft, byCombinedCode, "Combined code", reduced, log);
            reduced = true;
        }
        return byCombinedCode;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByMilitaryFlag(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, bool military, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        CAircraftModelList byMilitaryFlag(inList.findByMilitaryFlag(military));
        const QString mil(military ? "military" : "civilian");
        if (byMilitaryFlag.isEmpty())
        {
            if (log) { logDetails(log, remoteAircraft, "Not found by " + mil); }
            return inList;
        }

        if (log)
        {
            if (inList.size() > byMilitaryFlag.size())
            {
                logDetails(log, remoteAircraft, "Reduced to " + mil + " aircraft, size " + QString::number(byMilitaryFlag.size()));
            }
            else
            {
                logDetails(log, remoteAircraft, "Not reduced by " + mil + ", size " + QString::number(byMilitaryFlag.size()));
            }
        }
        return byMilitaryFlag;
    }

    void CAircraftMatcher::logDetails(CStatusMessageList *log, const CSimulatedAircraft &remoteAircraft, const QString &message, CStatusMessage::StatusSeverity s)
    {
        logDetails(log, remoteAircraft.getCallsign(), message, s);
    }

    void CAircraftMatcher::logDetails(CStatusMessageList *log, const CCallsign &callsign, const QString &message, CStatusMessage::StatusSeverity s)
    {
        if (!log) { return; }
        if (message.isEmpty()) { return; }
        const CStatusMessage m(getLogCategories(), s, callsign.isEmpty() ? callsign.toQString() + ": " + message.trimmed() : message.trimmed());
        log->push_back(m);
    }
} // namespace
