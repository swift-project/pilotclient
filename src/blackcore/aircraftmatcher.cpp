/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/aircraftmatcher.h"
#include "blackcore/matchingutils.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessagelist.h"

#include <QList>
#include <QStringList>
#include <QtGlobal>

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
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Manually set model " + remoteAircraft.getModelString(), getLogCategories());
            return remoteAircraft.getModel();
        }

        CAircraftModel matchedModel(remoteAircraft.getModel());
        if (matchedModels.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No models for matching, using default", getLogCategories(), CStatusMessage::SeverityError);
            matchedModel = this->getDefaultModel();
            matchedModel.setCallsign(remoteAircraft.getCallsign());
            return matchedModel;
        }

        do
        {
            if (matchedModels.isEmpty())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No models for matching, using default", getLogCategories(), CStatusMessage::SeverityWarning);
                matchedModel = this->getDefaultModel();
                break;
            }

            // try to find in installed models by model string
            if (mode.testFlag(ByModelString))
            {
                matchedModel = matchByExactModelString(remoteAircraft, matchedModels, log);
                if (matchedModel.hasModelString()) { break; }
            }
            else if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping model string match", getLogCategories());
            }

            // primary reduction
            bool reduced = false;
            do
            {
                // by livery, then by ICAO
                if (mode.testFlag(ByLivery))
                {
                    matchedModels = ifPossibleReduceByLiveryAndIcaoCode(remoteAircraft, matchedModels, reduced, log);
                    if (reduced) { break; }
                }
                else if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping livery reduction", getLogCategories());
                }

                // by ICAO data from set
                if (mode.testFlag(ByIcaoData))
                {
                    // if already matched by livery skip
                    matchedModels = ifPossibleReduceByIcaoData(remoteAircraft, matchedModels, false, reduced, log);
                    if (reduced) { break; }
                }
                else if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping ICAO reduction", getLogCategories());
                }

                // family
                if (mode.testFlag(ByFamily))
                {
                    QString family = remoteAircraft.getAircraftIcaoCode().getFamily();
                    matchedModels = ifPossibleReduceByFamily(remoteAircraft, family, matchedModels, "real family", reduced, log);
                    if (reduced) { break; }

                    // scenario: the ICAO actually is the family
                    family = remoteAircraft.getAircraftIcaoCodeDesignator();
                    matchedModels = ifPossibleReduceByFamily(remoteAircraft, family, matchedModels, "ICAO treated as family", reduced, log);
                    if (reduced) { break; }
                }
                else if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping family match", getLogCategories());
                }

                // combined code
                if (mode.testFlag(ByCombinedCode))
                {
                    matchedModels = ifPossibleReduceByCombinedCode(remoteAircraft, matchedModels, true, reduced, log);
                    if (reduced) { break; }
                }
                else if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping combined code match", getLogCategories());
                }
            }
            while (false);

            // here we have a list of possible models, we reduce/refine further
            bool military = remoteAircraft.getModel().isMilitary();
            matchedModels = ifPossibleReduceByManufacturer(remoteAircraft, matchedModels, "2nd pass", reduced, log);
            matchedModels = ifPossibleReduceByMilitaryFlag(remoteAircraft, military, matchedModels, reduced, log);

            // expect first to be the right one in order
            matchedModel = matchedModels.isEmpty() ? getDefaultModel() : matchedModels.front();
        }
        while (false);

        // copy over callsign and other data
        matchedModel.setCallsign(remoteAircraft.getCallsign());

        Q_ASSERT_X(!matchedModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign");
        Q_ASSERT_X(matchedModel.hasModelString(), Q_FUNC_INFO, "Missing model string");
        Q_ASSERT_X(matchedModel.getModelType() != CAircraftModel::TypeUnknown, Q_FUNC_INFO, "Missing model type");

        return matchedModel;
    }

    CAircraftModel CAircraftMatcher::reverselLookupModel(const CCallsign &callsign, const QString &networkAircraftIcao, const QString &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString, CAircraftModel::ModelType type, CStatusMessageList *log)
    {
        CLivery livery;
        livery.setAirlineIcaoCode(networkAirlineIcao);
        CAircraftModel model(networkModelString, type, "", CAircraftIcaoCode(networkAircraftIcao), livery);
        model.setCallsign(callsign);
        model = CAircraftMatcher::reverselLookupModel(model, networkLiveryInfo, log);
        return model;
    }

    CAircraftModel CAircraftMatcher::reverselLookupModel(const CAircraftModel &modelToLookup, const QString &networkLiveryInfo, CStatusMessageList *log)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        // already DB model?
        CAircraftModel model(modelToLookup);
        if (modelToLookup.hasValidDbKey() && modelToLookup.getModelType() == CAircraftModel::TypeDatabaseEntry) { return modelToLookup; }

        // --- now I try to fill in as many DB data as possible ---
        // 1) This will unify data where possible
        // 2) I have full information of what the other pilot flies where possible
        // 3) This is not model matching here (!), it is a process of getting the most accurate data from that fuzzy information I get via FSD
        //
        // reverse lookup, use DB data wherever possible
        // 1) If I cannot resolce the ICAO codes here, they are either wrong (most likely in most cases) or
        // 2) not in the DB yet

        const CCallsign callsign(model.getCallsign());
        const QStringList liveryModelStrings = CAircraftModel::splitNetworkLiveryString(networkLiveryInfo);
        const QString modelString(modelToLookup.hasModelString() ? modelToLookup.getModelString() : liveryModelStrings[1]);
        QString liveryCode(liveryModelStrings[0]);

        if (!modelString.isEmpty())
        {
            // if we find the model here we have a fully defined DB model
            const CAircraftModel modelFromDb(sApp->getWebDataServices()->getModelForModelString(modelString));
            if (modelFromDb.hasValidDbKey())
            {
                model = modelFromDb;
                if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup found DB model '%1' for '%2'").arg(modelFromDb.getModelStringAndDbKey()).arg(callsign.toQString()), getLogCategories()); }
            }
            else
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup did not find model for '%1'").arg(modelString), getLogCategories()); }
            }
        }

        if (!model.hasValidDbKey())
        {
            // only if not yet matched with DB
            CAircraftIcaoCode aircraftIcaoUsedForLookup(model.getAircraftIcaoCode());
            CAirlineIcaoCode airlineIcaoUsedForLookup(model.getAirlineIcaoCode());
            if (!aircraftIcaoUsedForLookup.hasValidDbKey())
            {
                aircraftIcaoUsedForLookup = CAircraftMatcher::reverseLookupAircraftIcao(aircraftIcaoUsedForLookup.getDesignator(), callsign, log);
            }

            if (!airlineIcaoUsedForLookup.hasValidDbKey())
            {
                airlineIcaoUsedForLookup = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcaoUsedForLookup.getDesignator(), callsign, log);
            }

            // try to match by livery
            if (liveryCode.isEmpty() && airlineIcaoUsedForLookup.hasValidDesignator())
            {
                // we create a standard livery code, then we try to find based on this
                liveryCode = CLivery::getStandardCode(model.getAirlineIcaoCode());
            }

            if (CLivery::isValidCombinedCode(liveryCode))
            {
                // search DB model by livery
                const CAircraftModelList models(sApp->getWebDataServices()->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftIcaoUsedForLookup.getDesignator(), liveryCode));
                if (models.isEmpty())
                {
                    // no models for that livery, search for livery only
                    const CLivery databaseLivery(sApp->getWebDataServices()->getLiveryForCombinedCode(liveryCode));
                    if (databaseLivery.hasValidDbKey())
                    {
                        // we have found a livery in the DB
                        model.setLivery(databaseLivery);
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of livery '%1'").arg(databaseLivery.getCombinedCodePlusInfo()), getLogCategories()); }
                    }
                }
                else
                {
                    // model by livery data found
                    model = models.front();
                    if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of DB model '%1' for '%2'/'%3'', found '%4'").arg(model.getDbKey()).arg(aircraftIcaoUsedForLookup.getDesignator()).arg(liveryCode).arg(models.size()), getLogCategories()); }
                }
            }

            // if no DB livery, create own dummy livery
            if (!model.hasValidDbKey() && !model.getLivery().hasValidDbKey())
            {
                // create a pseudo livery, try to find airline first
                CAirlineIcaoCode reverseAirlineIcao(sApp->getWebDataServices()->smartAirlineIcaoSelector(CAirlineIcaoCode(airlineIcaoUsedForLookup.getDesignator())));
                if (!reverseAirlineIcao.hasValidDbKey())
                {
                    // no DB data, we update as much as possible
                    reverseAirlineIcao = model.getAirlineIcaoCode();
                    reverseAirlineIcao.updateMissingParts(CAirlineIcaoCode(airlineIcaoUsedForLookup));
                }
                if (airlineIcaoUsedForLookup.hasValidDesignator())
                {
                    const CLivery liveryDummy(CLivery::getStandardCode(reverseAirlineIcao), reverseAirlineIcao, "Generated");
                    model.setLivery(liveryDummy);
                    if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup, set dummy livery `%1`").arg(liveryDummy.getCombinedCodePlusInfo())); }
                }
            }

            if (!model.getAircraftIcaoCode().hasValidDbKey())
            {
                CAircraftIcaoCode reverseAircraftIcao(sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(aircraftIcaoUsedForLookup.getDesignator()));
                if (!reverseAircraftIcao.hasValidDbKey())
                {
                    // no DB data, we update as much as possible
                    reverseAircraftIcao = model.getAircraftIcaoCode();
                    reverseAircraftIcao.updateMissingParts(CAircraftIcaoCode(aircraftIcaoUsedForLookup));
                    if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup, aircraft '%1' not found in DB").arg(reverseAircraftIcao.getDesignator())); }
                }
                if (reverseAircraftIcao.hasDesignator())
                {
                    model.setAircraftIcaoCode(reverseAircraftIcao);
                    if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup, set aircraft ICAO to '%1'").arg(reverseAircraftIcao.getCombinedIcaoStringWithKey())); }
                }
            }
        } // model from DB

        model.setCallsign(callsign);
        if (log)
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Uisng model: ICAO '%1', livery '%2', model '%3', type '%4'").
                                                arg(model.getAircraftIcaoCode().getCombinedIcaoStringWithKey()).
                                                arg(model.getLivery().getCombinedCodePlusInfo()).
                                                arg(model.getModelString()).
                                                arg(model.getModelTypeAsString())
                                               );
        }
        return model;
    }

    CAircraftIcaoCode CAircraftMatcher::reverseLookupAircraftIcao(const QString &icaoDesignator, const CCallsign &callsign, CStatusMessageList *log)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        const QString designator(icaoDesignator.trimmed().toUpper());
        const CAircraftIcaoCode icao = sApp->getWebDataServices()->smartAircraftIcaoSelector(designator);
        if (log)
        {
            if (icao.hasValidDbKey()) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup for ICAO '%1' found '%2'").arg(designator).arg(icao.getDesignator()), CAircraftMatcher::getLogCategories()); }
            else { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of ICAO '%1'', nothing found").arg(designator), CAircraftMatcher::getLogCategories()); }
        }
        return icao;
    }

    CAirlineIcaoCode BlackCore::CAircraftMatcher::reverseLookupAirlineIcao(const QString &icaoDesignator, const CCallsign &callsign, CStatusMessageList *log)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        const QString designator(icaoDesignator.trimmed().toUpper());
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->smartAirlineIcaoSelector(designator, callsign);
        if (log)
        {
            if (icao.hasValidDbKey()) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of airline '%1' and callsign '%2' found '%3'").arg(designator).arg(callsign.asString()).arg(icao.getDesignator()), CAircraftMatcher::getLogCategories()); }
            else { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of airline '%1' and callsign '%2', nothing found").arg(designator).arg(callsign.asString()), CAircraftMatcher::getLogCategories()); }
        }
        return icao;
    }

    CLivery CAircraftMatcher::reverseLookupStandardLivery(const CAirlineIcaoCode &airline, const CCallsign &callsign, CStatusMessageList *log)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        if (!airline.hasValidDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, "Reverse lookup of standard livery skipped, no airline designator", CAircraftMatcher::getLogCategories(), CStatusMessage::SeverityWarning); }
            return CLivery();
        }

        const CLivery livery = sApp->getWebDataServices()->getStdLiveryForAirlineCode(airline);
        if (log)
        {
            if (livery.hasValidDbKey()) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of standard livery for '%1' found '%2'").arg(airline.getDesignator()).arg(livery.getCombinedCode()), CAircraftMatcher::getLogCategories()); }
            else { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Not standard livery for airline '%1'").arg(airline.getDesignator()), CAircraftMatcher::getLogCategories()); }
        }
        return livery;
    }

    CAirlineIcaoCode CAircraftMatcher::callsignToAirline(const CCallsign &callsign, CStatusMessageList *log)
    {
        if (callsign.isEmpty() || !sApp || !sApp->getWebDataServices()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->findBestMatchByCallsign(callsign);

        if (icao.hasValidDesignator())
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Turned callsign %1 into airline %2").arg(callsign.asString()).arg(icao.getDesignator()), getLogCategories());
        }
        else
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Cannot turn callsign %1 into airline").arg(callsign.asString()), getLogCategories());
        }
        return icao;
    }

    int  CAircraftMatcher::setModelSet(const CAircraftModelList &models)
    {
        CAircraftModelList modelsCleaned(models);
        const int r1 = modelsCleaned.removeAllWithoutModelString();
        const int r2 = modelsCleaned.removeIfExcluded();
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No model string, no exact match possible"); }
            return CAircraftModel();
        }

        CAircraftModel model = models.findFirstByModelStringOrDefault(remoteAircraft.getModelString());
        if (log)
        {
            if (model.hasModelString())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found exact match for " + model.getModelString());
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No exact match for " + model.getModelString());
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No livery code, no reduction possible", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList byLivery(
            inList.findByAircraftDesignatorAndLiveryCombinedCode(
                remoteAircraft.getLivery().getCombinedCode(),
                remoteAircraft.getAircraftIcaoCodeDesignator()
            ));

        if (byLivery.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by livery code " + remoteAircraft.getLivery().getCombinedCode(), getLogCategories()); }
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No aircraft designator, skipping step", getLogCategories()); }
            return inList;
        }

        BlackMisc::Simulation::CAircraftModelList searchModels(inList.findByIcaoDesignators(
                    remoteAircraft.getAircraftIcaoCode(),
                    ignoreAirline ? CAirlineIcaoCode() : remoteAircraft.getAirlineIcaoCode()));

        if (!searchModels.isEmpty())
        {
            if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                    "Possible aircraft " + QString::number(searchModels.size()) +
                                                    ", found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " +
                                                    (ignoreAirline ? "" : remoteAircraft.getAirlineIcaoCodeDesignator()), getLogCategories());
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
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                        "Not found by ICAO " +
                                                        remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator() +
                                                        " relaxing to only ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator());
                }
                // recursive lookup by ignoring airline
                return ifPossibleReduceByIcaoData(remoteAircraft, inList, true, reduced, log);
            }
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by ICAO " + remoteAircraft.getAircraftIcaoCodeDesignator() + " " + remoteAircraft.getAirlineIcaoCodeDesignator()); }
        }
        return inList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, const CAircraftModelList &inList, const QString &hint, bool &reduced, CStatusMessageList *log)
    {
        // Use an algorithm to find the best match
        reduced = true;
        if (family.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No family, skipping step (" + hint + ")", getLogCategories()); }
            return inList;
        }
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No models for family match (" + hint + ")", getLogCategories()); }
            return inList;
        }

        CAircraftModelList found(inList.findByFamily(family));
        if (found.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by family " + family + " (" + hint + ")"); }
            return inList;
        }

        reduced = true;
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found by family " + family + " (" + hint + ") size " + QString::number(found.size()), getLogCategories());
        return found;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByManufacturer(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " " + "Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        const QString m = remoteAircraft.getAircraftIcaoCode().getManufacturer();
        if (m.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " No manufacturer, cannot reduce " + QString::number(inList.size()) +  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByManunfacturer(m));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " Not found " + m + ", cannot reduce", getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " Reduced by " + m + " results: " + QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAirline(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " " + "Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        if (!remoteAircraft.hasAirlineDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " " + "No airline, cannot reduce " + QString::number(inList.size()) +  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByIcaoDesignators(CAircraftIcaoCode(), remoteAircraft.getAirlineIcaoCode()));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " Cannot reduce by " + remoteAircraft.getAirlineIcaoCodeDesignator() + " results: " + QString::number(outList.size()), getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log , remoteAircraft, info + " Reduced reduce by " + remoteAircraft.getAirlineIcaoCodeDesignator() + " to " + QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByCombinedCode(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool relaxIfNotFound, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getAircraftIcaoCode().hasValidCombinedType())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No valid combined code", getLogCategories()); }
            return inList;
        }

        const QString cc = remoteAircraft.getAircraftIcaoCode().getCombinedType();
        CAircraftModelList byCombinedCode(inList.findByCombinedCode(cc));
        if (byCombinedCode.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by combined code " + cc, getLogCategories()); }
            if (relaxIfNotFound)
            {

            }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found by combined code " + cc + ", possible " + QString::number(byCombinedCode.size()), getLogCategories()); }
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by " + mil, getLogCategories()); }
            return inList;
        }

        if (log)
        {
            if (inList.size() > byMilitaryFlag.size())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Reduced to " + mil + " aircraft, size " + QString::number(byMilitaryFlag.size()), getLogCategories());
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not reduced by " + mil + ", size " + QString::number(byMilitaryFlag.size()), getLogCategories());
            }
        }
        return byMilitaryFlag;
    }
} // namespace
