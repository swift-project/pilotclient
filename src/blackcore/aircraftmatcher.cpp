/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/aircraftmatcher.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/matchingscript.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "aircraftmatcher.h"

#include <QList>
#include <QStringList>
#include <QtGlobal>
#include <QPair>
#include <QStringBuilder>
#include <QJSEngine>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    const QStringList &CAircraftMatcher::getLogCategories()
    {
        static const QStringList cats { CLogCategories::matching() };
        return cats;
    }

    CAircraftMatcher::CAircraftMatcher(const CAircraftMatcherSetup &setup, QObject *parent) : QObject(parent), m_setup(setup)
    {
        if (sApp && sApp->hasWebDataServices())
        {
            sApp->getWebDataServices()->synchronizeDbCaches(CEntityFlags::AircraftCategoryEntity);
            const CAircraftCategoryList categories = sApp->getWebDataServices()->getAircraftCategories();
            m_categoryMatcher.setCategories(categories);
        }
    }

    CAircraftMatcher::CAircraftMatcher(QObject *parent) : CAircraftMatcher(CAircraftMatcherSetup(), parent)
    { }

    CAircraftMatcher::~CAircraftMatcher()
    {
        this->saveDisabledForMatchingModels();
    }

    bool CAircraftMatcher::setSetup(const CAircraftMatcherSetup &setup)
    {
        if (m_setup == setup) { return false; }
        m_setup = setup;
        emit this->setupChanged();
        return true;
    }

    CAirlineIcaoCode CAircraftMatcher::failoverValidAirlineIcaoDesignator(
        const CCallsign &callsign, const QString &primaryIcao, const QString &secondaryIcao,
        bool airlineFromCallsign,
        const QString &airlineName, const QString &airlineTelephony,
        bool useWebServices, CStatusMessageList *log)
    {
        CCallsign::addLogDetailsToList(log, callsign,
                                           QStringLiteral("Find airline designator from 1st: '%1' 2nd: '%2' callsign: '%3', use airline callsign: %4, name: '%5' telephony: '%6' use web service: %7").
                                           arg(primaryIcao, secondaryIcao, callsign.toQString(), boolToYesNo(airlineFromCallsign), airlineName, airlineTelephony, boolToYesNo(useWebServices)), getLogCategories());
        CAirlineIcaoCode code;
        do
        {
            if (CAircraftMatcher::isValidAirlineIcaoDesignator(primaryIcao, useWebServices))
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using primary airline ICAO '%1'").arg(primaryIcao), getLogCategories());
                code = stringToAirlineIcaoObject(callsign, primaryIcao, airlineName, airlineTelephony, useWebServices, log);
                break;
            }
            if (CAircraftMatcher::isValidAirlineIcaoDesignator(secondaryIcao, useWebServices))
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using secondary airline ICAO '%1', primary '%2' not valid").arg(secondaryIcao, primaryIcao), getLogCategories());
                code = stringToAirlineIcaoObject(callsign, secondaryIcao, airlineName, airlineTelephony, useWebServices, log);
                break;
            }

            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Two invalid airline ICAO codes (primary/secondary) '%1', '%2'").arg(primaryIcao, secondaryIcao), getLogCategories());
            if (airlineFromCallsign)
            {
                QString flightNumber;
                const QString airlinePrefix = callsign.getAirlinePrefix(flightNumber);
                if (airlinePrefix.isEmpty() || flightNumber.isEmpty())
                {
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Callsign '%1' cannot be split in airline '%1'/ flight number '%2'").arg(callsign.toQString(), flightNumber), getLogCategories());
                    break;
                }
                if (CAircraftMatcher::isValidAirlineIcaoDesignator(airlinePrefix, useWebServices))
                {
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using airline from callsign '%1', suffix: '%2'").arg(callsign.toQString(), airlinePrefix), getLogCategories());
                    code = stringToAirlineIcaoObject(callsign, airlinePrefix, airlineName, airlineTelephony, useWebServices, log);
                    break;
                }
            }
        }
        while (false);

        if (code.hasValidDesignator())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Resolved to airline designator: %1").arg(code.toQString(true)));
        }
        else
        {

            CCallsign::addLogDetailsToList(log, callsign,
                                               QStringLiteral("Cannot find airline designator from 1st: '%1' 2nd: '%2' callsign: '%3', use airline callsign: %4, name: '%5' telephony: '%6' use web service: %7").
                                               arg(primaryIcao, secondaryIcao, callsign.toQString(), boolToYesNo(airlineFromCallsign), airlineName, airlineTelephony, boolToYesNo(useWebServices)), getLogCategories());
        }
        return code;
    }

    CAirlineIcaoCode CAircraftMatcher::failoverValidAirlineIcaoDesignator(const CCallsign &callsign, const QString &primaryIcao, const QString &secondaryIcao, bool airlineFromCallsign, const QString &airlineName, const QString &airlineTelephony, const CAircraftModelList &models, CStatusMessageList *log)
    {
        CCallsign::addLogDetailsToList(log, callsign,
                                           QStringLiteral("Find airline designator from 1st: '%1' 2nd: '%2' callsign: '%3', use airline callsign: %4, airline name: '%5' telephony: '%6', models: %7").
                                           arg(primaryIcao, secondaryIcao, callsign.toQString(), boolToYesNo(airlineFromCallsign), airlineName, airlineTelephony, models.sizeString()), getLogCategories());

        if (models.isEmpty())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("No models to find airline from"));
            return CAirlineIcaoCode();
        }

        static const QString info("Multiple models (%1) with airline ICAOs for '%2'");
        CAirlineIcaoCode code;

        do
        {
            bool reduced = false;
            if (!primaryIcao.isEmpty())
            {
                CAircraftModelList modelsWithAirline = models.findByIcaoDesignators({}, primaryIcao);
                const QMap<CAirlineIcaoCode, int> countPerAirline = modelsWithAirline.countPerAirlineIcao();
                if (countPerAirline.size() == 1)
                {
                    code = countPerAirline.firstKey();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found only 1 airline ICAO '%1' in %2 models").arg(countPerAirline.firstKey().getDesignatorDbKey()).arg(models.size()), getLogCategories());
                    break;
                }

                if (modelsWithAirline.size() > 0)
                {
                    if (modelsWithAirline.size() > 1)
                    {
                        modelsWithAirline = CAircraftMatcher::ifPossibleReduceModelsByAirlineNameTelephonyDesignator(callsign, airlineName, airlineTelephony, modelsWithAirline, info.arg(modelsWithAirline.size()).arg(primaryIcao), reduced, log);
                    }
                    code = modelsWithAirline.getAirlineWithMaxCount();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using primary airline ICAO '%1' found '%2'").arg(primaryIcao, code.getDesignatorDbKey()), getLogCategories());
                    break;
                }
            }

            if (!secondaryIcao.isEmpty())
            {
                CAircraftModelList modelsWithAirline = models.findByIcaoDesignators({}, secondaryIcao);
                const QMap<CAirlineIcaoCode, int> countPerAirline = modelsWithAirline.countPerAirlineIcao();
                if (countPerAirline.size() == 1)
                {
                    code = countPerAirline.firstKey();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found only 1 airline ICAO '%1' in %2 models").arg(countPerAirline.firstKey().getDesignatorDbKey()).arg(models.size()), getLogCategories());
                    break;
                }

                if (modelsWithAirline.size() > 0)
                {
                    if (modelsWithAirline.size() > 1)
                    {
                        modelsWithAirline = CAircraftMatcher::ifPossibleReduceModelsByAirlineNameTelephonyDesignator(callsign, airlineName, airlineTelephony, modelsWithAirline, info.arg(modelsWithAirline.size()).arg(secondaryIcao), reduced, log);
                    }
                    code = modelsWithAirline.getAirlineWithMaxCount();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using secondary airline ICAO '%1' found '%2'").arg(primaryIcao, code.getDesignatorDbKey()), getLogCategories());
                    break;
                }
            }

            if (airlineFromCallsign)
            {
                QString flightNumber;
                const QString airlinePrefix = callsign.getAirlinePrefix(flightNumber);
                if (airlinePrefix.isEmpty() || flightNumber.isEmpty())
                {
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Callsign '%1' cannot be split in airline '%1'/ flight number '%2'").arg(callsign.toQString(), flightNumber), getLogCategories());
                    break;
                }

                CAircraftModelList modelsWithAirline = models.findByIcaoDesignators({}, airlinePrefix);
                const QMap<CAirlineIcaoCode, int> countPerAirline = modelsWithAirline.countPerAirlineIcao();
                if (countPerAirline.size() == 1)
                {
                    code = countPerAirline.firstKey();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found only 1 airline ICAO '%1' in %2 models").arg(countPerAirline.firstKey().getDesignatorDbKey()).arg(models.size()), getLogCategories());
                    break;
                }
                if (modelsWithAirline.size() > 0)
                {
                    if (modelsWithAirline.size() > 1)
                    {
                        modelsWithAirline = CAircraftMatcher::ifPossibleReduceModelsByAirlineNameTelephonyDesignator(callsign, airlineName, airlineTelephony, modelsWithAirline, info.arg(modelsWithAirline.size()).arg(airlinePrefix), reduced, log);
                    }
                    code = modelsWithAirline.getAirlineWithMaxCount();
                    CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using callsign airline ICAO '%1' found '%2'").arg(airlinePrefix, code.getDesignatorDbKey()), getLogCategories());
                    break;
                }
            }
        }
        while (false);

        // return message
        if (code.hasValidDesignator())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Resolved to airline designator: %1").arg(code.toQString(true)));
        }
        else
        {
            CCallsign::addLogDetailsToList(log, callsign,
                                               QStringLiteral("Cannot find airline designator from 1st: '%1' 2nd: '%2' callsign: '%3', use airline callsign: %4, airline name: '%5' telephony: '%6', models: %7").
                                               arg(primaryIcao, secondaryIcao, callsign.toQString(), boolToYesNo(airlineFromCallsign), airlineName, airlineTelephony, models.sizeString()), getLogCategories());
        }
        return code;
    }

    CAirlineIcaoCode CAircraftMatcher::failoverValidAirlineIcaoDesignatorModelsFirst(const CCallsign &callsign, const QString &primaryIcao, const QString &secondaryIcao, bool airlineFromCallsign, const QString &airlineName, const QString &airlineTelephony, const CAircraftModelList &models, CStatusMessageList *log)
    {
        if (!models.isEmpty())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using %1 models to resolve airline designator").arg(models.size()));
            const CAirlineIcaoCode airline = CAircraftMatcher::failoverValidAirlineIcaoDesignator(callsign, primaryIcao, secondaryIcao, airlineFromCallsign, airlineName, airlineTelephony, models, log);
            if (airline.hasValidDbKey()) { return airline; }
        }
        CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Now using resolution of airline ICAO without specific models"));
        return CAircraftMatcher::failoverValidAirlineIcaoDesignator(callsign, primaryIcao, secondaryIcao, airlineFromCallsign, airlineName, airlineTelephony, true, log);
    }

    CAircraftModel CAircraftMatcher::getClosestMatch(const CSimulatedAircraft &remoteAircraft, MatchingLog whatToLog, CStatusMessageList *log, bool useMatchingScript) const
    {
        CAircraftModelList modelSet(m_modelSet); // Models for this matching
        const CAircraftMatcherSetup setup = m_setup;

        static const QString format("hh:mm:ss.zzz");
        static const QString m1("--- Start matching: UTC %1 ---");
        static const QString m2("Input model: '%1' '%2'");
        static const QString m3("Matching uses model set of %1 models\n%2");
        static const QString m4("Setup %1");
        static const QString summary(
            "Matching summary\n"
            "-----------------------------------------\n"
            "- Combined: %1 -> %2\n"
            "- Aircraft: %3 -> %4\n"
            "- Airline:  %5 -> %6\n"
            "- Livery:   %7 -> %8\n"
            "- Model:    %9 -> %10\n"
            "- Script modifed value: %11\n"
            "-----------------------------------------\n");

        const QDateTime startTime = QDateTime::currentDateTimeUtc();
        if (whatToLog == MatchingLogNothing) { log = nullptr; }
        if (log) { log->clear(); }

        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m1.arg(startTime.toString(format)));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m2.arg(remoteAircraft.getCallsignAsString(), removeSurroundingApostrophes(remoteAircraft.getModel().toQString())));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m3.arg(modelSet.size()).arg(modelSet.coverageSummaryForModel(remoteAircraft.getModel())));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m4.arg(setup.toQString(true)));

        // Before I really search I check some special conditions
        // 1) Manually set model (by user)
        // 2) No model set at all
        // 3) Exact match by model string

        // Manually set string?
        CAircraftModel matchedModel;
        bool resolvedInPrephase = false;
        if (remoteAircraft.getModel().hasManuallySetString())
        {
            // the user did a manual mapping "by hand", so he really should know what he is doing
            // no matching
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Manually set model " % remoteAircraft.getModelString(), getLogCategories());
            matchedModel = remoteAircraft.getModel();
            resolvedInPrephase = true;
        }
        else if (modelSet.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No models for matching, using default"), getLogCategories(), CStatusMessage::SeverityError);
            matchedModel = this->getDefaultModel();
            resolvedInPrephase = true;
        }
        else if (remoteAircraft.hasModelString())
        {
            // try to find in installed models by model string
            if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByModelString))
            {
                matchedModel = matchByExactModelString(remoteAircraft, modelSet, whatToLog, log);
                if (matchedModel.hasModelString())
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Exact match by model string '" % matchedModel.getModelStringAndDbKey() % "'", getLogCategories(), CStatusMessage::SeverityError);
                    resolvedInPrephase = true;
                }
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping model string match", getLogCategories());
            }
        }

        if (!resolvedInPrephase)
        {
            // sanity
            const int noString = modelSet.removeAllWithoutModelString();
            static const QString noModelStr("Excluded %1 models without model string");
            if (noString > 0 && log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, noModelStr.arg(noString)); }

            // exclusion
            if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ExcludeNoDbData))
            {
                const int noDbKey = modelSet.removeObjectsWithoutDbKey();
                static const QString excludedStr("Excluded %1 models without DB key");
                if (noDbKey > 0 && log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, excludedStr.arg(noDbKey)); }
            }

            if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ExcludeNoExcluded))
            {
                const int excluded = modelSet.removeIfExcluded();
                static const QString excludedStr("Excluded %1 models marked 'Excluded'");
                if (excluded > 0 && log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, excludedStr.arg(excluded)); }
            }

            // Reduce by ICAO if the flag is set
            static const QString msInfo("Using '%1' with model set with %2 models");
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, msInfo.arg(setup.getMatchingAlgorithmAsString()).arg(modelSet.size()), getLogCategories());
            CAircraftModelList candidates;
            int maxScore = -1;

            switch (setup.getMatchingAlgorithm())
            {
            case CAircraftMatcherSetup::MatchingStepwiseReduce:
                candidates = CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(modelSet, setup, m_categoryMatcher, remoteAircraft, whatToLog, log);
                break;
            case CAircraftMatcherSetup::MatchingScoreBased:
                candidates = CAircraftMatcher::getClosestMatchScoreImplementation(modelSet, setup, remoteAircraft, maxScore, whatToLog, log);
                break;
            case CAircraftMatcherSetup::MatchingStepwiseReducePlusScoreBased:
            default:
                candidates = CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(modelSet, setup, m_categoryMatcher, remoteAircraft, whatToLog, log);
                candidates = CAircraftMatcher::getClosestMatchScoreImplementation(candidates, setup, remoteAircraft, maxScore, whatToLog, log);
                break;
            }

            if (candidates.isEmpty())
            {
                matchedModel = CAircraftMatcher::getCombinedTypeDefaultModel(modelSet, remoteAircraft, this->getDefaultModel(), whatToLog, log);
            }
            else
            {
                CAircraftMatcherSetup::PickSimilarStrategy usedStrategy = setup.getPickStrategy();
                switch (usedStrategy)
                {
                case CAircraftMatcherSetup::PickRandom:
                    matchedModel = candidates.randomElement<CAircraftModel>();
                    break;
                case CAircraftMatcherSetup::PickByOrder:
                    if (!candidates.needsOrder())
                    {
                        matchedModel = candidates.minOrderOrDefault();
                        break;
                    }
                    [[fallthrough]];
                case CAircraftMatcherSetup::PickFirst: // fallthru intentionally
                default:
                    usedStrategy = CAircraftMatcherSetup::PickFirst; // re-assigned if fall-through
                    matchedModel = candidates.front();
                    break;
                }

                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Picking among %1 by strategy '%2'").arg(candidates.size()).arg(CAircraftMatcherSetup::strategyToString(usedStrategy))); }
            }
        }

        // copy over callsign validate
        matchedModel.setCallsign(remoteAircraft.getCallsign());
        matchedModel.setModelType(CAircraftModel::TypeModelMatching);

        // matching script
        bool didRunAndModifyMatchingScript = false;
        if (useMatchingScript && setup.doRunMsMatchingStageScript())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Matching script: Matching stage script used"));
            const MatchingScriptReturnValues rv = CAircraftMatcher::matchingStageScript(remoteAircraft.getModel(), matchedModel, setup, modelSet, log);
            CAircraftModel matchedModelMs = matchedModel;

            if (rv.runScriptAndModified())
            {
                matchedModelMs = rv.model;
                didRunAndModifyMatchingScript = true;
            }

            if (rv.runScriptModifiedAndRerun())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Matching script: Modified values and re-run requested"));
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Matching script: Now using model: '%1'").arg(matchedModel.toQString(true)));

                CSimulatedAircraft rerunAircraft(remoteAircraft);
                rerunAircraft.setModel(matchedModelMs);
                CStatusMessageList log2ndRun;
                matchedModelMs = CAircraftMatcher::getClosestMatch(rerunAircraft, whatToLog, log ? &log2ndRun : nullptr, false);
                if (log) { log->push_back(log2ndRun); }

                // the script can fuckup the model, leading to an empty model string or such
                matchedModelMs.setCallsign(remoteAircraft.getCallsign());
                if (matchedModelMs.hasModelString())
                {
                    matchedModel = matchedModelMs;
                }
                else
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Matching script: Ignoring model without model string after running the script"));
                }
            }
        }
        else
        {
            if (log) CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Matching script: No matching stage script used"));
        }

        // copy over callsign validate (again, just in case it was changed in matching script)
        matchedModel.setCallsign(remoteAircraft.getCallsign());
        matchedModel.setModelType(CAircraftModel::TypeModelMatching);

        // reported here by LT: https://discordapp.com/channels/539048679160676382/539925070550794240/701439918815051846
        if (!matchedModel.hasModelString())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("All matching yielded no result, VERY odd...")); }
            CAircraftModel defaultModel = this->getDefaultModel();
            if (defaultModel.hasModelString())
            {
                matchedModel = defaultModel;
                matchedModel.setCallsign(remoteAircraft.getCallsign());
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Using default model '%1'").arg(matchedModel.getModelString())); }
            }
            else
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Not even a default model. Giving up").arg(matchedModel.getModelString())); }
            }
        }

        Q_ASSERT_X(!matchedModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign for matched model");
        // Q_ASSERT_X(matchedModel.hasModelString(), Q_FUNC_INFO, "Missing model string for matched model");

        if (log)
        {
            static const QString nms = "no model string";
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                summary.arg(
                                                    remoteAircraft.getAircraftIcaoCode().getCombinedType(),    matchedModel.getAircraftIcaoCode().getCombinedType(),
                                                    remoteAircraft.getAircraftIcaoCode().getDesignatorDbKey(), matchedModel.getAircraftIcaoCode().getDesignatorDbKey(),
                                                    remoteAircraft.getAirlineIcaoCode().getVDesignatorDbKey(), matchedModel.getAirlineIcaoCode().getVDesignatorDbKey()
                                                ).arg(
                                                    remoteAircraft.getLivery().getCombinedCodePlusInfoAndId(), matchedModel.getLivery().getCombinedCodePlusInfoAndId(),
                                                    defaultIfEmpty(remoteAircraft.getModel().getModelStringAndDbKey(), nms), matchedModel.getModelStringAndDbKey(),
                                                    boolToYesNo(didRunAndModifyMatchingScript)
                                                ));
        } // log

        const QDateTime endTime = QDateTime::currentDateTimeUtc();
        const qint64 matchingTime = startTime.msecsTo(endTime);
        static const QString em("--- Matching end: UTC %1, time %2ms ---");
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, em.arg(endTime.toString(format)).arg(matchingTime));
        return matchedModel;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModel(
        const CCallsign &callsign, const CAircraftIcaoCode &networkAircraftIcao,
        const CAirlineIcaoCode &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString,
        const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, CAircraftModel::ModelType type, CStatusMessageList *log)
    {
        Q_UNUSED(setup)

        CLivery livery;
        livery.setAirlineIcaoCode(networkAirlineIcao);

        CAircraftModel model(networkModelString, type, {}, networkAircraftIcao, livery);
        model.setCallsign(callsign);
        model = CAircraftMatcher::reverseLookupModel(model, networkLiveryInfo, setup, modelSet, log);
        model.setModelType(CAircraftModel::TypeReverseLookup);

        return model;
    }

    MatchingScriptReturnValues CAircraftMatcher::reverseLookupScript(const CAircraftModel &inModel, const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, CStatusMessageList *log)
    {
        if (!setup.doRunMsReverseLookupScript()) { return MatchingScriptReturnValues(inModel); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return inModel; }
        const QString js = CFileUtils::readFileToString(setup.getMsReverseLookupFile());
        const MatchingScriptReturnValues rv = CAircraftMatcher::matchingScript(js, inModel, inModel, setup, modelSet, ReverseLookup, log);
        return rv;
    }

    MatchingScriptReturnValues CAircraftMatcher::matchingStageScript(const CAircraftModel &inModel, const CAircraftModel &matchedModel, const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, CStatusMessageList *log)
    {
        if (!setup.doRunMsMatchingStageScript()) { return MatchingScriptReturnValues(inModel); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return inModel; }
        const QString js = CFileUtils::readFileToString(setup.getMsMatchingStageFile());
        const MatchingScriptReturnValues rv = CAircraftMatcher::matchingScript(js, inModel, matchedModel, setup, modelSet, MatchingStage, log);
        return rv;
    }

    MatchingScriptReturnValues CAircraftMatcher::matchingScript(const QString &js, const CAircraftModel &inModel, const CAircraftModel &matchedModel, const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, MatchingScript script, CStatusMessageList *log)
    {
        MatchingScriptReturnValues rv(inModel);
        QString logMessage;
        const CCallsign callsign = inModel.getCallsign();

        if (js.isEmpty() && log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script is empty")); }

        while (!js.isEmpty() && sApp && sApp->hasWebDataServices())
        {
            rv.runScript = true;

            // matching script
            const bool msReverse = (script == ReverseLookup);
            const QString lf = msReverse ? setup.getMsReverseLookupFile() : setup.getMsMatchingStageFile();
            static const QString logFileR = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), "logMatchingScriptReverseLookup.log");
            static const QString logFileM = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), "logMatchingScriptMatchingStage.log");

            if (log)
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script (%1): '%2'").arg(msToString(script), lf));
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script input model (%1): '%2'").arg(inModel.toQString(true)));
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script models: %1").arg(modelSet.coverageSummary()));
            }

            QJSEngine engine;
            // engine.installExtensions(QJSEngine::ConsoleExtension);

            // Meta objects to create new JS objects, here causing JSValue can't be reassigned to another engine.
            // static const QJSValue jsInOutMetaObject = engine.newQMetaObject(&MSInOutValues::staticMetaObject);
            // engine.globalObject().setProperty("SwiftInOutObject", jsInOutMetaObject); // JS: new SwiftInOutObject();
            // static const QJSValue jsSetMetaObject = engine.newQMetaObject(&MSModelSet::staticMetaObject);
            // engine.globalObject().setProperty("SwiftModelSet", jsSetMetaObject); // JS: new SwiftModelSet
            // static const QJSValue jsWebServicesMetaObject = engine.newQMetaObject(&MSWebServices::staticMetaObject);
            // engine.globalObject().setProperty("SwiftWebServices", jsWebServicesMetaObject); // JS: new SwiftWebServices

            // init models and set
            MSInOutValues inObject(inModel);
            MSInOutValues matchedObject(matchedModel); // same as inModel for reverse lookup
            matchedObject.evaluateChanges(inModel.getAircraftIcaoCode(), inModel.getAirlineIcaoCode());
            MSInOutValues outObject(matchedModel);     // set default values for out object
            MSModelSet modelSetObject(modelSet);       // as passed
            modelSetObject.initByAircraftAndAirline(inModel.getAircraftIcaoCode(), inModel.getAirlineIcaoCode());
            MSWebServices webServices; // web services encapsulated

            // object as from network
            const QJSValue jsInObject = engine.newQObject(&inObject);
            engine.globalObject().setProperty("inObject", jsInObject);

            // object that will be returned
            const QJSValue jsOutObject = engine.newQObject(&outObject);
            engine.globalObject().setProperty("outObject", jsOutObject);

            // object as matched so far, same as inObject in reverse lookup
            const QJSValue jsMatchedObject = engine.newQObject(&matchedObject);
            engine.globalObject().setProperty("matchedObject", jsMatchedObject);

            // wrapper for model set
            const QJSValue jsModelSetObject = engine.newQObject(&modelSetObject);
            engine.globalObject().setProperty("modelSet", jsModelSetObject);

            // wrapper for web services
            const QJSValue jsWebServices = engine.newQObject(&webServices);
            engine.globalObject().setProperty("webServices", jsWebServices);

            QJSValue ms = engine.evaluate(js, msReverse ? logFileR : logFileM);
            ms = ms.call();
            if (ms.isError())
            {
                const QString msg = QStringLiteral("Matching script error: %1 '%2'").arg(ms.property("lineNumber").toInt()).arg(ms.toString());
                CLogMessage(static_cast<CAircraftMatcher *>(nullptr)).warning(msg);
                if (log) { CCallsign::addLogDetailsToList(log, callsign, msg); }
            }
            else
            {
                if (ms.isQObject())
                {
                    const MSInOutValues *reverseModelProcessed = qobject_cast<const MSInOutValues *>(ms.toQObject());
                    logMessage = reverseModelProcessed->getLogMessage();
                    if (!reverseModelProcessed->isModified()) { break; }

                    // rerun
                    rv.rerun = reverseModelProcessed->isRerun();

                    // changed model by model id?
                    if (reverseModelProcessed->hasChangedModelId(inModel))
                    {
                        const CAircraftModel model = sApp->getWebDataServices()->getModelForDbKey(reverseModelProcessed->getDbModelId());
                        if (model.hasValidDbKey())
                        {
                            // found full model from DB
                            rv.model = model;
                            rv.modified = true;
                            break;
                        }
                    }

                    // changed model by model string?
                    if (reverseModelProcessed->hasChangedModelString(inModel.getModelString()))
                    {
                        const QString modelString = reverseModelProcessed->getModelString();
                        const CAircraftModel model = sApp->getWebDataServices()->getModelForModelString(modelString);
                        if (model.hasValidDbKey())
                        {
                            // found full model from DB
                            rv.model    = model;
                            rv.modified = true;
                            break;
                        }

                        // search for model string in set, even if it is not in the DB
                        const CAircraftModel modeSetModel = CAircraftMatcher::reverseLookupModelStringInSet(modelString, callsign, modelSet, true, log);
                        if (modeSetModel.hasModelString())
                        {
                            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script using model from set: '%1'").arg(modelString));

                            // NON DB model from model set
                            rv.model    = modeSetModel;
                            rv.modified = true;
                            break;
                        }
                    }

                    // changed aircraft ICAO
                    if (reverseModelProcessed->hasChangedAircraftIcao(matchedModel.getAircraftIcaoCode()))
                    {
                        CAircraftIcaoCode icao(reverseModelProcessed->getAircraftIcao());
                        if (reverseModelProcessed->hasChangedAircraftIcaoId(matchedModel.getAircraftIcaoCode()))
                        {
                            icao = sApp->getWebDataServices()->getAircraftIcaoCodeForDbKey(reverseModelProcessed->getDbAircraftIcaoId());
                        }
                        rv.modified = true;
                        rv.model.setAircraftIcaoCode(icao);
                    }

                    if (reverseModelProcessed->hasChangedLiveryId(matchedModel.getLivery()))
                    {
                        const CLivery livery(sApp->getWebDataServices()->getLiveryForDbKey(reverseModelProcessed->getDbLiveryId()));
                        rv.model.setLivery(livery);
                        rv.modified = true;
                    }
                    else if (reverseModelProcessed->hasChangedAirlineIcao(matchedModel.getAirlineIcaoCode()))
                    {
                        CAirlineIcaoCode icao;
                        if (reverseModelProcessed->hasChangedAirlineIcaoId(matchedModel.getAirlineIcaoCode()))
                        {
                            icao = sApp->getWebDataServices()->getAirlineIcaoCodeForDbKey(reverseModelProcessed->getDbAirlineIcaoId());
                        }
                        else
                        {
                            icao = sApp->getWebDataServices()->getAirlineIcaoCodeForUniqueDesignatorOrDefault(reverseModelProcessed->getAirlineIcao(), true);
                        }

                        const CLivery livery(sApp->getWebDataServices()->getStdLiveryForAirlineCode(icao));
                        rv.model.setLivery(livery);
                        if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script, changed airline ICAO: '%1' -> '%2'").arg(matchedModel.getAirlineIcaoCode().toQString(true), icao.toQString(true))); }
                        rv.modified = true;
                    }
                }
                else if (ms.isString())
                {
                    logMessage = ms.toString();
                }
            }

            // end this
            break;
        }

        // log message
        if (log && !logMessage.isEmpty()) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Matching script log: '%1'").arg(logMessage)); }

        // end
        return rv;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModel(const CAircraftModel &modelToLookup, const QString &networkLiveryInfo, const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftModel(); }

        // already DB model?
        CAircraftModel model(modelToLookup); // copy
        if (modelToLookup.isLoadedFromDb()) { return modelToLookup; }

        // --- now I try to fill in as many DB data as possible ---
        // 1) This will unify data where possible
        // 2) I have full information of what the other pilot flies where possible
        // 3) This is not model matching here (!), it is a process of getting the most accurate data from that fuzzy information I get via FSD
        //
        // Reverse lookup, use DB data wherever possible
        // 1) If I cannot resolve the ICAO codes here, they are either wrong (most likely in most cases) or
        // 2) not in the DB yet
        const CCallsign callsign(model.getCallsign());

        do
        {
            if (modelToLookup.hasModelString())
            {
                if (!setup.isReverseLookupModelString())
                {
                    if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Model string looup disabled")); }
                }
                else
                {
                    const QString modelString = modelToLookup.getModelString();

                    // if we find the model here we have a fully defined DB model
                    const CAircraftModel modelFromDb = CAircraftMatcher::reverseLookupModelStringInDB(modelString, callsign, setup.isReverseLookupModelString(), log);
                    if (modelFromDb.hasValidDbKey())
                    {
                        model = modelFromDb;
                        break; // done here
                    }

                    // const bool useNonDbEntries = setup.isDbDataOnly();
                    const bool useNonDbEntries = true;
                    const CAircraftModel modelFromSet = CAircraftMatcher::reverseLookupModelStringInSet(modelString, callsign, modelSet, useNonDbEntries, log);
                    if (modelFromSet.hasModelString())
                    {
                        model = modelFromSet;
                        break; // done here
                    }
                }
            }

            // lookup if model is not yet from DB
            const DBTripleIds ids = CAircraftModel::parseNetworkLiveryString(networkLiveryInfo);
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Livery string with ids: '%1'").arg(ids.toQString())); }

            if (!setup.isReverseLookupSwiftLiveryIds())
            {
                if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Ignoring livery ids '%1', because of setup").arg(ids.toQString())); }
            }
            else if (ids.model >= 0 && !modelToLookup.hasModelString())
            {
                if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Model lookup with id %1 from triple ids '%2'").arg(ids.model).arg(ids.toQString())); }
                const CAircraftModel modelFromDb = CAircraftMatcher::reverseLookupModelId(ids.model, callsign, log);
                if (modelFromDb.hasValidDbKey())
                {
                    model = modelFromDb;
                    break;
                }
            }

            // no direct resolution of model, try livery and aircraft ICAO
            if (!modelToLookup.getAircraftIcaoCode().hasValidDbKey() && ids.aircraft >= 0)
            {
                if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Aircraft ICAO lookup with id %1 from triple ids '%2'").arg(ids.aircraft).arg(ids.toQString())); }
                const CAircraftIcaoCode icaoFromDb = CAircraftMatcher::reverseLookupAircraftIcaoId(ids.aircraft, callsign, log);
                if (icaoFromDb.hasValidDbKey())
                {
                    model.setAircraftIcaoCode(icaoFromDb);
                }
            }

            if (!modelToLookup.getLivery().hasValidDbKey() && ids.livery >= 0)
            {
                if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Livery lookup with id %1 from triple ids '%2'").arg(ids.livery).arg(ids.toQString())); }
                const CLivery liveryFromDb = CAircraftMatcher::reverseLookupLiveryId(ids.livery, callsign, log);
                if (liveryFromDb.hasValidDbKey())
                {
                    model.setLivery(liveryFromDb);
                }
            }

            // aircraft ICAO if not from DB yet
            if (!model.getAircraftIcaoCode().hasValidDbKey())
            {
                CAircraftIcaoCode reverseIcaoCode(model.getAircraftIcaoCode());
                if (!reverseIcaoCode.isLoadedFromDb())
                {
                    reverseIcaoCode = CAircraftMatcher::reverseLookupAircraftIcao(reverseIcaoCode, callsign, log);
                    if (reverseIcaoCode.isLoadedFromDb())
                    {
                        if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Set aircraft ICAO to '%1' from DB").arg(reverseIcaoCode.getCombinedIcaoStringWithKey())); }
                        model.setAircraftIcaoCode(reverseIcaoCode);
                    }
                    else
                    {
                        // no DB data
                        if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup, ICAO '%1' not resolved from DB").arg(reverseIcaoCode.getDesignator())); }
                    }
                }
            }

            // check if livery is already from DB
            if (!model.getLivery().isLoadedFromDb())
            {
                CAirlineIcaoCode airlineIcaoCode(model.getAirlineIcaoCode());
                if (!airlineIcaoCode.isLoadedFromDb())
                {
                    airlineIcaoCode = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcaoCode, callsign, log);
                }

                // try to match by livery
                QString liveryCode = networkLiveryInfo;
                if (liveryCode.isEmpty() && airlineIcaoCode.hasValidDesignator())
                {
                    // we create a standard livery code, then we try to find based on this
                    liveryCode = CLivery::getStandardCode(airlineIcaoCode);
                }

                if (CLivery::isValidCombinedCode(liveryCode))
                {
                    // search livery by combined code
                    const CLivery reverseLivery(sApp->getWebDataServices()->getLiveryForCombinedCode(liveryCode));
                    if (reverseLivery.hasValidDbKey())
                    {
                        // we have found a livery in the DB
                        model.setLivery(reverseLivery);
                        if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of livery found '%1'").arg(reverseLivery.getCombinedCodePlusInfoAndId()), getLogCategories()); }
                    }
                    else
                    {
                        // no livery data found
                        if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of livery '%1' yielded no result").arg(reverseLivery.getCombinedCodePlusInfo()), getLogCategories()); }
                    }
                } // livery lookup

                // if no DB livery yet, create own livery
                if (!model.hasValidDbKey() && !model.getLivery().hasValidDbKey())
                {
                    if (airlineIcaoCode.hasValidDesignator())
                    {
                        if (airlineIcaoCode.hasValidDbKey())
                        {
                            const CLivery stdLivery(sApp->getWebDataServices()->getStdLiveryForAirlineCode(airlineIcaoCode));
                            if (stdLivery.hasValidDbKey())
                            {
                                model.setLivery(stdLivery);
                                if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Set standardlivery `%1`").arg(stdLivery.getCombinedCodePlusInfo())); }
                            }
                        }

                        if (!model.getLivery().hasValidDbKey())
                        {
                            // create a pseudo livery, try to find airline first
                            const CLivery liveryDummy(CLivery::getStandardCode(airlineIcaoCode), airlineIcaoCode, "Generated");
                            model.setLivery(liveryDummy);
                            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Generated livery, set livery `%1`").arg(liveryDummy.getCombinedCodePlusInfo())); }
                        }
                    }
                } // pseudo livery
            } // livery from DB
        }
        while (false);

        model.setCallsign(callsign);
        model.setModelType(modelToLookup.getModelType());

        if (log)
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Using model: ICAO '%1', livery '%2', model '%3', type '%4'").arg(
                                                   model.getAircraftIcaoCode().getCombinedIcaoStringWithKey(),
                                                   model.getLivery().getCombinedCodePlusInfo(),
                                                   model.getModelString(), model.getModelTypeAsString()));
        }
        return model;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModelMs(const CAircraftModel &modelToLookup, const QString &networkLiveryInfo, const CAircraftMatcherSetup &setup, const CAircraftModelList &modelSet, CStatusMessageList *log)
    {
        CAircraftModel reverseModel = reverseLookupModel(modelToLookup, networkLiveryInfo, setup, modelSet, log);
        if (!setup.doRunMsReverseLookupScript()) { return reverseModel; }
        const CCallsign cs = modelToLookup.getCallsign();
        const MatchingScriptReturnValues rv = CAircraftMatcher::reverseLookupScript(reverseModel, setup, modelSet, log);
        if (rv.runScriptModifiedAndRerun())
        {
            CCallsign::addLogDetailsToList(log, cs, QStringLiteral("Matching script: Modified value and requested rerun"));

            // no script the 2nd time
            CAircraftMatcherSetup setupRerun(setup);
            setupRerun.resetReverseLookup();
            reverseModel = CAircraftMatcher::reverseLookupModel(rv.model, networkLiveryInfo, setupRerun, modelSet, log);
            return reverseModel;
        }
        return (rv.runScriptAndModified() ? rv.model : reverseModel);
    }

    CAircraftModel CAircraftMatcher::reverseLookupModelStringInDB(const QString &modelString, const CCallsign &callsign, bool doLookupString, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftModel(); }
        if (!doLookupString)
        {
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Ignore model string in reverse lookup (disabled), ignoring '%1'").arg(modelString)); }
            return CAircraftModel();
        }
        CAircraftModel model = sApp->getWebDataServices()->getModelForModelString(modelString);
        const bool isDBModel = model.hasValidDbKey();
        if (log)
        {
            if (isDBModel)
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found model in DB for model string '%1' dist: '%2' descr.: '%3'").arg(model.getModelStringAndDbKey(), model.getDistributor().getDbKey(), model.getDescription()));
            }
            else
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Did not find model in DB for model string '%1'").arg(modelString));
            }
        }

        if (!isDBModel) { return CAircraftModel(); } // not found

        // found
        model.setCallsign(callsign);
        model.setModelType(CAircraftModel::TypeReverseLookup);
        return model;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModelStringInSet(const QString &modelString, const CCallsign &callsign, const CAircraftModelList &modelSet, bool useNonDbEntries, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftModel(); }
        if (modelString.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Empty model string for lookup in %1 models").arg(modelSet.size())); }
            return CAircraftModel();
        }
        if (modelSet.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Empty models, ignoring '%1'").arg(modelString)); }
            return CAircraftModel();
        }

        CAircraftModel model = modelSet.findFirstByModelStringOrDefault(modelString, Qt::CaseInsensitive);
        if (!model.hasModelString())
        {
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Model '%1' not found in %2 models").arg(modelString).arg(modelSet.size())); }
            return CAircraftModel();
        }

        const bool isDBModel = model.hasValidDbKey();
        if (log)
        {
            if (isDBModel)
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found DB model in %1 models for model string '%2'").arg(modelSet.size()).arg(model.getModelStringAndDbKey()));
            }
            else
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found NON DB model in %1 models for model string '%2'").arg(modelSet.size()).arg(model.getModelString()));
            }
        }

        if (!isDBModel && !useNonDbEntries) { return CAircraftModel(); } // ignore DB entries

        // found
        model.setCallsign(callsign);
        model.setModelType(isDBModel ? CAircraftModel::TypeReverseLookup : CAircraftModel::TypeOwnSimulatorModel);
        return model;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModelId(int id, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftModel(); }
        CAircraftModel model = sApp->getWebDataServices()->getModelForDbKey(id);
        if (log)
        {
            if (model.hasValidDbKey())
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Found model in DB for id '%1'").arg(id));
            }
            else
            {
                CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Did not find model in DB for id '%1'").arg(id));
            }
        }
        model.setCallsign(callsign);
        model.setModelType(CAircraftModel::TypeReverseLookup);
        return model;
    }

    CAircraftIcaoCode CAircraftMatcher::reverseLookupAircraftIcao(const CAircraftIcaoCode &icaoCandidate, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftIcaoCode(); }

        const QString designator(icaoCandidate.getDesignator());
        CAircraftIcaoCodeList foundIcaos = sApp->getWebDataServices()->getAircraftIcaoCodesForDesignator(designator);

        if (foundIcaos.isEmpty())
        {
            CAircraftIcaoCode icao(designator);

            // sometimes from network we receive something like "CESSNA C172"
            if (CAircraftIcaoCode::isValidDesignator(designator))
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of aircraft ICAO '%1' did not find anything, using smart search").arg(designator), CAircraftMatcher::getLogCategories());
                icao = sApp->getWebDataServices()->smartAircraftIcaoSelector(icaoCandidate);
            }
            else
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of invalid ICAO code '%1' did not find anything so far").arg(designator), CAircraftMatcher::getLogCategories());
                const QStringList parts(designator.split(' '));
                for (const QString &p : parts)
                {
                    CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Trying parts, now reverse lookup of aircraft ICAO '%1' using smart search").arg(p), CAircraftMatcher::getLogCategories());
                    icao = sApp->getWebDataServices()->smartAircraftIcaoSelector(p);
                    if (icao.isLoadedFromDb()) break;
                }
            }
            if (icao.isLoadedFromDb())
            {
                // smart search found DB data
                foundIcaos = CAircraftIcaoCodeList({icao});
            }
            else
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("No DB data for ICAO '%1', valid ICAO?").arg(designator), CAircraftMatcher::getLogCategories());
                return CAircraftIcaoCode(icaoCandidate);
            }
        }

        if (foundIcaos.size() < 1)
        {
            CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of aircraft ICAO '%1', nothing found").arg(designator), CAircraftMatcher::getLogCategories());
            return CAircraftIcaoCode(icaoCandidate);
        }
        else if (foundIcaos.size() == 1)
        {
            const CAircraftIcaoCode icao(foundIcaos.front());
            CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of aircraft ICAO '%1', found one manufacturer '%2' in DB").arg(designator, icao.getDesignatorManufacturer()), CAircraftMatcher::getLogCategories());
            return icao;
        }
        else
        {
            // multiple ICAOs
            Q_ASSERT_X(foundIcaos.size() > 1, Q_FUNC_INFO, "Wrong size");
            const QPair<QString, int> maxManufacturer = foundIcaos.maxCountManufacturer();
            CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of aircraft ICAO '%1', found %2 values (ambiguous): %3").arg(designator).arg(foundIcaos.size()).arg(foundIcaos.dbKeysAsString(", ")), CAircraftMatcher::getLogCategories());
            if (maxManufacturer.second < foundIcaos.size())
            {
                foundIcaos = foundIcaos.findByManufacturer(maxManufacturer.first);
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reducing by manufacturer '%1', now %2 values").arg(maxManufacturer.first).arg(foundIcaos.size()), CAircraftMatcher::getLogCategories());
            }
            foundIcaos.sortByRank();
            const CAircraftIcaoCode icao = foundIcaos.front(); // best rank
            CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Reverse lookup of aircraft ICAO '%1', using ICAO '%2' with rank %3").arg(designator, icao.toQString(), icao.getRankString()), CAircraftMatcher::getLogCategories());
            return icao;
        }
    }

    CAircraftIcaoCode CAircraftMatcher::reverseLookupAircraftIcaoId(int id, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftIcaoCode(); }
        const CAircraftIcaoCode icao = sApp->getWebDataServices()->getAircraftIcaoCodeForDbKey(id);
        if (log)
        {
            if (icao.hasValidDbKey())
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Found aircraft ICAO in DB for id '%1'").arg(id));
            }
            else
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Did not find aircraft ICAO in DB for id '%1'").arg(id));
            }
        }
        return icao;
    }

    CAirlineIcaoCode CAircraftMatcher::reverseLookupAirlineIcao(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->smartAirlineIcaoSelector(icaoPattern, callsign);
        if (log)
        {
            if (icao.hasValidDbKey()) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of airline ICAO '%1' and callsign '%2' found '%3' '%4' in DB").arg(icaoPattern.getDesignator(), callsign.asString(), icao.getVDesignatorDbKey(), icao.getName()), CAircraftMatcher::getLogCategories()); }
            else { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of airline ICAO '%1' and callsign '%2', nothing found in DB").arg(icaoPattern.getDesignator(), callsign.asString()), CAircraftMatcher::getLogCategories()); }
        }
        return icao;
    }

    CLivery CAircraftMatcher::reverseLookupStandardLivery(const CAirlineIcaoCode &airline, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CLivery(); }
        if (!airline.hasValidDesignator())
        {
            if (log) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of standard livery skipped, no airline designator"), CAircraftMatcher::getLogCategories(), CStatusMessage::SeverityWarning); }
            return CLivery();
        }

        const CLivery livery = sApp->getWebDataServices()->getStdLiveryForAirlineCode(airline);
        if (log)
        {
            if (livery.hasValidDbKey()) { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Reverse lookup of standard livery for '%1' found '%2'").arg(airline.getDesignator(), livery.getCombinedCode()), CAircraftMatcher::getLogCategories()); }
            else { CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Not standard livery for airline '%1' in DB").arg(airline.getDesignator()), CAircraftMatcher::getLogCategories()); }
        }
        return livery;
    }

    CLivery CAircraftMatcher::reverseLookupLiveryId(int id, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CLivery(); }
        const CLivery livery = sApp->getWebDataServices()->getLiveryForDbKey(id);
        if (log)
        {
            if (livery.hasValidDbKey())
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Found livery in DB for id '%1'").arg(id));
            }
            else
            {
                CCallsign::addLogDetailsToList(log, logCallsign, QStringLiteral("Did not find livery in DB for id '%1'").arg(id));
            }
        }
        return livery;
    }

    int CAircraftMatcher::reverseLookupByIds(const DBTripleIds &ids, CAircraftIcaoCode &aircraftIcao, CLivery &livery, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        int found = 0;
        if (ids.livery >= 0)
        {
            livery = CAircraftMatcher::reverseLookupLiveryId(ids.livery, logCallsign, log);
            if (livery.hasValidDbKey()) { found++; }
        }

        if (ids.aircraft >= 0)
        {
            aircraftIcao = CAircraftMatcher::reverseLookupAircraftIcaoId(ids.aircraft, logCallsign, log);
            if (aircraftIcao.hasValidDbKey()) { found++; }
        }
        return found;
    }

    QString CAircraftMatcher::reverseLookupAirlineName(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return {}; }
        if (candidate.isEmpty()) { return {}; }
        const QStringList names = sApp->getWebDataServices()->getAirlineNames();
        if (names.contains(candidate, Qt::CaseInsensitive))
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Airline name '%1' found in DB").arg(candidate));
            return candidate;
        }

        CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Airline name '%1' not found in DB").arg(candidate));
        return {};
    }

    QString CAircraftMatcher::reverseLookupTelephonyDesignator(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return {}; }
        if (candidate.isEmpty()) { return {}; }
        const QStringList designators = sApp->getWebDataServices()->getTelephonyDesignators();
        if (designators.contains(candidate, Qt::CaseInsensitive))
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Airline name '%1' found").arg(candidate));
            return candidate;
        }

        CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Airline name '%1' not found").arg(candidate));
        return {};
    }

    bool CAircraftMatcher::isKnownAircraftDesignator(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return false; }
        if (!CAircraftIcaoCode::isValidDesignator(candidate))
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("No valid ICAO designator '%1'").arg(candidate));
            return false;
        }

        const bool known = sApp->getWebDataServices()->containsAircraftIcaoDesignator(candidate);
        static const QString sKnown("Known ICAO designator '%1'");
        static const QString sUnknown("Unknown ICAO designator '%1'");
        CCallsign::addLogDetailsToList(log, callsign, known ? sKnown.arg(candidate) : sUnknown.arg(candidate));
        return known;
    }

    bool CAircraftMatcher::isKnownModelString(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return false; }
        const bool known = sApp->getWebDataServices()->containsModelString(candidate);
        static const QString sKnown("Known modelstring '%1'");
        static const QString sUnknown("Unknown modelstring '%1'");
        CCallsign::addLogDetailsToList(log, callsign, known ? sKnown.arg(candidate) : sUnknown.arg(candidate));
        return known;
    }

    CAircraftIcaoCode CAircraftMatcher::searchAmongAirlineAircraft(const QString &candidateString, const CAirlineIcaoCode &airline, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftIcaoCode(); }
        if (!airline.isLoadedFromDb())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("No valid airline from DB '%1'").arg(airline.getDesignator()));
            return CAircraftIcaoCode();
        }

        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        const CAircraftIcaoCodeList aircraft = sApp->getWebDataServices()->getAircraftIcaoCodesForAirline(airline);
        if (aircraft.isEmpty())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("No aircraft known for airline '%1'").arg(airline.getDesignator()));
            return CAircraftIcaoCode();
        }

        const QSet<QString> allIcaos = aircraft.allDesignators();
        const QString allIcaosStr = allIcaos.values().join(", ");
        CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Aircraft '%1' known for airline '%2'").arg(allIcaosStr, airline.getDesignator()));

        const CAircraftIcaoCode code = aircraft.findBestFuzzyMatchOrDefault(candidateString);
        if (code.hasValidDesignator())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Aircraft '%1' is best fuzzy search of '%2' for airline '%3'").arg(code.toQString(), candidateString, airline.getDesignator()));
            return code;
        }

        return aircraft.front();
    }

    CAirlineIcaoCode CAircraftMatcher::callsignToAirline(const CCallsign &callsign, CStatusMessageList *log)
    {
        if (callsign.isEmpty() || !sApp || !sApp->getWebDataServices()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->findBestMatchByCallsign(callsign);

        if (icao.hasValidDesignator())
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Turned callsign %1 into airline %2").arg(callsign.asString(), icao.getDesignator()), getLogCategories());
        }
        else
        {
            CCallsign::addLogDetailsToList(log, callsign, QStringLiteral("Cannot turn callsign %1 into airline").arg(callsign.asString()), getLogCategories());
        }
        return icao;
    }

    int CAircraftMatcher::setModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator, bool forced)
    {
        if (!simulator.isSingleSimulator()) { return 0; }

        CAircraftModelList modelsCleaned(models);
        const int r1 = modelsCleaned.removeAllWithoutModelString();
        const int r2 = modelsCleaned.removeIfExcluded();

        // sane simulator and model count? (minor risk of not updating when a model was changed)
        if (!forced && m_simulator == simulator && m_modelSet.size() == modelsCleaned.size())
        {
            return m_modelSet.size();
        }

        QString warnings;
        if ((r1 + r2) > 0)
        {
            warnings = QStringLiteral("Removed models for matcher, without string #: %1, excluded #: %2.").arg(r1).arg(r2);
            if (r1 > 0) { warnings += QStringLiteral(" Without string: '%1'.").arg(models.findEmptyModelStrings().getModelStringList().join(", ")); }
            if (r2 > 0) { warnings += QStringLiteral(" Excluded: '%1'.").arg(models.findByModelMode(CAircraftModel::Exclude).getModelStringList().join(", ")); }
        }
        const CAircraftModelList duplicateModels = modelsCleaned.findDuplicateModelStrings();

        // Warning info
        if (modelsCleaned.isEmpty())
        {
            // error to force popup
            CLogMessage(this).error(u"No models for matching ('%1'), swift without a model set will not work!") << simulator.toQString();
        }
        else if (!duplicateModels.isEmpty())
        {
            CLogMessage(this).error(u"Found model duplicate strings, check models: '%1'") << duplicateModels.dbKeysAsString(", ");
        }
        else if (!warnings.isEmpty())
        {
            CLogMessage(this).validationWarning(warnings);
        }
        else
        {
            CLogMessage(this).validationInfo(u"Set %1 models in matcher, simulator '%2'") << modelsCleaned.size() << simulator.toQString();
        }

        // set values
        m_modelSet  = modelsCleaned;
        m_simulator = simulator;
        m_modelSetInfo = QStringLiteral("Set: '%1' entries: %2").arg(simulator.toQString()).arg(modelsCleaned.size());
        return models.size();
    }

    void CAircraftMatcher::disableModelsForMatching(const CAircraftModelList &removedModels, bool incremental)
    {
        if (incremental)
        {
            m_modelSet.removeModelsWithString(removedModels, Qt::CaseInsensitive);
            m_disabledModels.push_back(removedModels);
        }
        else
        {
            this->restoreDisabledModels();
            m_disabledModels = removedModels;
            m_modelSet.removeModelsWithString(removedModels, Qt::CaseInsensitive);
        }
    }

    void CAircraftMatcher::restoreDisabledModels()
    {
        m_modelSet.replaceOrAddModelsWithString(m_disabledModels, Qt::CaseInsensitive);
    }

    void CAircraftMatcher::setDefaultModel(const CAircraftModel &defaultModel)
    {
        m_defaultModel = defaultModel;
        m_defaultModel.setModelType(CAircraftModel::TypeModelMatchingDefaultModel);
    }

    void CAircraftMatcher::evaluateStatisticsEntry(const QString &sessionId, const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
    {
        Q_UNUSED(livery)
        Q_ASSERT_X(sApp && sApp->hasWebDataServices(), Q_FUNC_INFO, "Missing web data services");
        if (m_modelSet.isEmpty())   { return; } // ignore empty sets to not create silly stats
        if (sessionId.isEmpty())    { return; }
        if (aircraftIcao.isEmpty()) { return; }

        QString description;
        if (!sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(aircraftIcao).isLoadedFromDb())
        {
            description = QStringLiteral("ICAO: '%1' not known, typo?").arg(aircraftIcao);
        }

        // resolve airline, mostly needed because of vPilot not sending airline ICAO codes in version 1
        CAirlineIcaoCode airlineIcaoChecked(airlineIcao);
        if (airlineIcao.isEmpty())
        {
            const CAirlineIcaoCode al = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcao, callsign);
            if (al.isLoadedFromDb())
            {
                airlineIcaoChecked = al;
            }
        }

        CMatchingStatisticsEntry::EntryType type = CMatchingStatisticsEntry::Missing;
        if (airlineIcaoChecked.hasValidDesignator())
        {
            type = m_modelSet.containsModelsWithAircraftAndAirlineIcaoDesignator(aircraftIcao, airlineIcao) ?
                   CMatchingStatisticsEntry::Found :
                   CMatchingStatisticsEntry::Missing;
        }
        else
        {
            type = m_modelSet.containsModelsWithAircraftAndAirlineIcaoDesignator(aircraftIcao, airlineIcao) ?
                   CMatchingStatisticsEntry::Found :
                   CMatchingStatisticsEntry::Missing;

        }
        m_statistics.addAircraftAirlineCombination(type, sessionId, m_modelSetInfo, description, aircraftIcao, airlineIcao);
    }

    void CAircraftMatcher::addingRemoteModelFailed(const CSimulatedAircraft &remoteAircraft)
    {
        if (!m_setup.removeFromSetIfFailed()) { return; }
        if (remoteAircraft.hasCallsign() && remoteAircraft.hasModelString())
        {
            const QString modelString = remoteAircraft.getModelString();
            const CAircraftModelList disabledModels({ remoteAircraft.getModel() });
            this->disableModelsForMatching(disabledModels, true);
            CLogMessage(this).warning(u"Disabled CS: '%1' model '%2' for matching") << remoteAircraft.getCallsignAsString() << modelString;
        }
        else
        {
            CLogMessage(this).warning(u"Disabled '%1' for matching") << remoteAircraft.toQString(true);
        }
    }

    bool CAircraftMatcher::saveDisabledForMatchingModels()
    {
        if (m_disabledModels.isEmpty()) { return false; }

        // log the models
        const QString ts = QDateTime::currentDateTimeUtc().toString("yyyyMMddHHmmss");
        const QString json = m_disabledModels.toJsonString();
        return CFileUtils::writeStringToFile(json, CFileUtils::appendFilePathsAndFixUnc(CSwiftDirectories::logDirectory(), QStringLiteral("removed models %1.json").arg(ts)));
    }

    CAircraftModelList CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(const CAircraftModelList &modelSet, const CAircraftMatcherSetup &setup, const CCategoryMatcher &categoryMatcher, const CSimulatedAircraft &remoteAircraft, MatchingLog whatToLog, CStatusMessageList *log)
    {
        CAircraftModelList matchedModels(modelSet);
        CAircraftModel matchedModel(remoteAircraft.getModel());
        Q_UNUSED(whatToLog)

        const CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        CStatusMessageList *reduceLog = log && whatToLog.testFlag(MatchingLogStepwiseReduce) ? log : nullptr;
        bool reduced = false;
        do
        {
            // by livery, then by ICAO
            if (mode.testFlag(CAircraftMatcherSetup::ByLivery))
            {
                matchedModels = ifPossibleReduceByLiveryAndAircraftIcaoCode(remoteAircraft, matchedModels, reduced, log);
                if (reduced) { break; } // almost perfect, we stop here (we have ICAO + livery match)
            }
            else if (reduceLog)
            {
                CMatchingUtils::addLogDetailsToList(reduceLog, remoteAircraft, QStringLiteral("Skipping livery reduction"), getLogCategories());
            }

            if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByIcaoData))
            {
                // by airline/aircraft or by aircraft/airline depending on setup
                // family is also considered
                matchedModels = ifPossibleReduceByIcaoData(remoteAircraft, matchedModels, setup, reduced, log);
            }
            else if (reduceLog)
            {
                CMatchingUtils::addLogDetailsToList(reduceLog, remoteAircraft, QStringLiteral("Skipping ICAO reduction"), getLogCategories());

                // family only because aircraft ICAO is not used
                if (mode.testFlag(CAircraftMatcherSetup::ByFamily))
                {
                    QString usedFamily;
                    matchedModels = ifPossibleReduceByFamily(remoteAircraft, UsePseudoFamily, matchedModels, reduced, usedFamily, log);
                    if (reduced) { break; }
                }
                else if (reduceLog)
                {
                    CMatchingUtils::addLogDetailsToList(reduceLog, remoteAircraft, QStringLiteral("Skipping family match"), getLogCategories());
                }
            }

            if (setup.useCategoryMatching())
            {
                matchedModels = categoryMatcher.reduceByCategories(matchedModels, modelSet, setup, remoteAircraft, reduced, whatToLog, log);
                // ?? break here ??
            }
            else if (reduceLog)
            {
                CMatchingUtils::addLogDetailsToList(reduceLog, remoteAircraft, QStringLiteral("category matching disabled"), getLogCategories());
            }

            // if not yet reduced, reduce to VTOL
            if (!reduced && remoteAircraft.isVtol() && matchedModels.containsVtol() && mode.testFlag(CAircraftMatcherSetup::ByVtol))
            {
                matchedModels = matchedModels.findByVtolFlag(true);
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Aircraft is VTOL, reduced to VTOL"), getLogCategories());
            }

            // military / civilian
            bool milFlagReduced = false;
            if (mode.testFlag(CAircraftMatcherSetup::ByMilitary) && remoteAircraft.isMilitary())
            {
                matchedModels = ifPossibleReduceByMilitaryFlag(remoteAircraft, matchedModels, reduced, reduceLog);
                milFlagReduced = true;
            }

            if (!milFlagReduced && mode.testFlag(CAircraftMatcherSetup::ByCivilian) && !remoteAircraft.isMilitary())
            {
                matchedModels = ifPossibleReduceByMilitaryFlag(remoteAircraft, matchedModels, reduced, reduceLog);
                milFlagReduced = true;
            }

            // combined code
            if (mode.testFlag(CAircraftMatcherSetup::ByCombinedType))
            {
                matchedModels = ifPossibleReduceByCombinedType(remoteAircraft, matchedModels, setup, reduced, reduceLog);
                if (reduced) { break; }
            }
            else if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Skipping combined code match"), getLogCategories());
            }
        }
        while (false);

        // here we have a list of possible models, we reduce/refine further
        if (matchedModels.size() > 1 && mode.testFlag(CAircraftMatcherSetup::ByManufacturer))
        {
            matchedModels = ifPossibleReduceByManufacturer(remoteAircraft, matchedModels, QStringLiteral("2nd trial to reduce by manufacturer. "), reduced, reduceLog);
        }

        return matchedModels;
    }

    CAircraftModelList CAircraftMatcher::getClosestMatchScoreImplementation(const CAircraftModelList &modelSet, const CAircraftMatcherSetup &setup, const CSimulatedAircraft &remoteAircraft, int &maxScore, MatchingLog whatToLog, CStatusMessageList *log)
    {
        CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        const bool noZeroScores = mode.testFlag(CAircraftMatcherSetup::ScoreIgnoreZeros);
        const bool preferColorLiveries = mode.testFlag(CAircraftMatcherSetup::ScorePreferColorLiveries);
        CStatusMessageList *scoreLog = log && whatToLog.testFlag(MatchingLogScoring) ? log : nullptr;

        // VTOL
        ScoredModels map;
        map = modelSet.scoreFull(remoteAircraft.getModel(), preferColorLiveries, noZeroScores, scoreLog);

        CAircraftModel matchedModel;
        if (map.isEmpty()) { return CAircraftModelList(); }

        maxScore = map.lastKey();
        const CAircraftModelList maxScoreAircraft(map.values(maxScore));
        CMatchingUtils::addLogDetailsToList(scoreLog, remoteAircraft, QStringLiteral("Scores: %1").arg(scoresToString(map)), getLogCategories());
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Scoring with score %1 out of %2 models yielded %3 models").arg(maxScore).arg(map.size()).arg(maxScoreAircraft.size()), getLogCategories());
        return maxScoreAircraft;
    }

    CAircraftModel CAircraftMatcher::getCombinedTypeDefaultModel(const CAircraftModelList &modelSet, const CSimulatedAircraft &remoteAircraft, const CAircraftModel &defaultModel, MatchingLog whatToLog, CStatusMessageList *log)
    {
        const QString combinedType = remoteAircraft.getAircraftIcaoCombinedType();
        CStatusMessageList *combinedLog = log && whatToLog.testFlag(MatchingLogCombinedDefaultType) ? log : nullptr;

        if (combinedType.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(combinedLog, remoteAircraft, QStringLiteral("No combined type, using default"), getLogCategories(), CStatusMessage::SeverityInfo);
            return defaultModel;
        }
        if (modelSet.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(combinedLog, remoteAircraft, QStringLiteral("No models, using default"), getLogCategories(), CStatusMessage::SeverityError);
            return defaultModel;
        }

        CMatchingUtils::addLogDetailsToList(combinedLog, remoteAircraft, u"Searching by combined type with color livery '" % combinedType % "'", getLogCategories());
        CAircraftModelList matchedModels = modelSet.findByCombinedTypeWithColorLivery(combinedType);
        if (!matchedModels.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(combinedLog, remoteAircraft, u"Found " % QString::number(matchedModels.size()) % u" by combined type w/color livery '" % combinedType % "'", getLogCategories());
        }
        else
        {
            CMatchingUtils::addLogDetailsToList(combinedLog, remoteAircraft, u"Searching by combined type '" % combinedType % "'", getLogCategories());
            matchedModels = matchedModels.findByCombinedType(combinedType);
            if (!matchedModels.isEmpty())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found " % QString::number(matchedModels.size()) % u" by combined '" % combinedType % "'", getLogCategories());
            }
        }

        // return
        if (matchedModels.isEmpty()) { return defaultModel; }
        return matchedModels.front();
    }

    CAircraftModel CAircraftMatcher::matchByExactModelString(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &models, MatchingLog whatToLog, CStatusMessageList *log)
    {
        CStatusMessageList *msLog = log && whatToLog.testFlag(MatchingLogModelstring) ? log : nullptr;
        if (remoteAircraft.getModelString().isEmpty())
        {
            if (msLog) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No model string, no exact match possible")); }
            return CAircraftModel();
        }

        CAircraftModel model = models.findFirstByModelStringAliasOrDefault(remoteAircraft.getModelString());
        if (msLog)
        {
            if (model.hasModelString())
            {
                CMatchingUtils::addLogDetailsToList(msLog, remoteAircraft, QStringLiteral("Found exact match for '%1'").arg(model.getModelString()));
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(msLog, remoteAircraft, QStringLiteral("No exact match for '%1'").arg(remoteAircraft.getModelString()));
            }
        }
        model.setModelType(CAircraftModel::TypeModelMatching);
        model.setCallsign(remoteAircraft.getCallsign());
        return model;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByLiveryAndAircraftIcaoCode(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getLivery().hasCombinedCode())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No livery code, no reduction possible"), getLogCategories()); }
            return inList;
        }

        const CAircraftModelList byLivery(
            inList.findByAircraftDesignatorAndLiveryCombinedCode(
                remoteAircraft.getLivery().getCombinedCode(),
                remoteAircraft.getAircraftIcaoCodeDesignator()
            ));

        if (byLivery.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Not found by livery code " % remoteAircraft.getLivery().getCombinedCode(), getLogCategories()); }
            return inList;
        }
        reduced = true;
        return byLivery;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByIcaoData(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const CAircraftMatcherSetup &setup, bool &reduced, CStatusMessageList *log)
    {
        const CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Empty list, skipping step"), getLogCategories()); }
            return inList;
        }

        reduced = false;
        if (mode.testFlag(CAircraftMatcherSetup::ByIcaoOrderAirlineFirst))
        {
            bool r1 = false;
            bool r2 = false;
            CAircraftModelList models = ifPossibleReduceByAirline(remoteAircraft, inList, setup, QStringLiteral("Reduce by airline first."), r1, log);
            models = ifPossibleReduceByAircraftOrFamily(remoteAircraft, UsePseudoFamily, models, setup, QStringLiteral("Reduce by aircraft ICAO second."), r2, log);
            reduced = r1 || r2;
            if (reduced) { return models; }
        }
        else if (mode.testFlag(CAircraftMatcherSetup::ByIcaoData))
        {
            bool r1 = false;
            bool r2 = false;
            CAircraftModelList models = ifPossibleReduceByAircraftOrFamily(remoteAircraft, UsePseudoFamily, inList, setup, QStringLiteral("Reduce by aircraft ICAO first."), r1, log);
            models = ifPossibleReduceByAirline(remoteAircraft, models, setup, QStringLiteral("Reduce aircraft ICAO by airline second."), r2, log);

            // not finding anything so far means we have no valid aircraft/airline ICAO combination
            // but it can happen we found B738, and for DLH there is no B738 but B737, so we search again
            if (!remoteAircraft.hasAirlineDesignator())
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No airline, no secondary search for airline/aircraft", getLogCategories()); }
            }
            else if (!r2 && mode.testFlag(CAircraftMatcherSetup::ByFamily))
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"No exact ICAO match of '" % remoteAircraft.getAirlineAndAircraftIcaoCodeDesignators() % u"', will try family combination", getLogCategories()); }

                bool r3 = false;
                QString usedFamily;
                CAircraftModelList models2nd = ifPossibleReduceByFamily(remoteAircraft, UsePseudoFamily, inList, r3, usedFamily, log);
                models2nd = ifPossibleReduceByAirline(remoteAircraft, models2nd, setup, "Reduce family by airline second.", r3, log);
                if (r3)
                {
                    // we found family / airline combination
                    if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found " % QString::number(models2nd.sizeInt()) % " aircraft family/airline '" % usedFamily % u"' combination", getLogCategories()); }
                    return models2nd;
                }
            }

            reduced = r1 || r2;
            if (reduced)
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Reduced by aircraft ICAO: " % boolToYesNo(r1) % u" airline: " % boolToYesNo(r2), getLogCategories()); }
                return models;
            }
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No reduction by ICAO data"), getLogCategories()); }
        return inList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByFamily(const CSimulatedAircraft &remoteAircraft, bool allowPseudoFamily, const CAircraftModelList &inList, bool &reduced, QString &usedFamily, CStatusMessageList *log)
    {
        reduced = false;
        usedFamily = remoteAircraft.getAircraftIcaoCode().getFamily();
        if (!usedFamily.isEmpty())
        {
            CAircraftModelList matchedModels = ifPossibleReduceByFamily(remoteAircraft, usedFamily, allowPseudoFamily, inList, QStringLiteral("real family from ICAO"), reduced, log);
            if (reduced) { return matchedModels; }
        }

        // scenario: the ICAO actually is the family
        usedFamily = remoteAircraft.getAircraftIcaoCodeDesignator();
        return ifPossibleReduceByFamily(remoteAircraft, usedFamily, allowPseudoFamily, inList, QStringLiteral("ICAO treated as family"), reduced, log);
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, bool allowPseudoFamily, const CAircraftModelList &inList, const QString &hint, bool &reduced, CStatusMessageList *log)
    {
        // Use an algorithm to find the best match
        reduced = false;
        if (family.isEmpty() && !allowPseudoFamily)
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"No family, skipping step (" % hint % u")", getLogCategories()); }
            return inList;
        }

        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"No models for family match (" % hint % u")", getLogCategories()); }
            return inList;
        }

        CAircraftModelList foundByFamily(inList.findByFamily(family));
        if (foundByFamily.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Not found by family '" % family % u"' (" % hint % ")"); }
            if (!allowPseudoFamily) { return inList; }
            // fallthru
        }
        else
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found by family '" % family % u"' (" % hint % u") size " % QString::number(foundByFamily.sizeInt()), getLogCategories()); }
        }

        CAircraftModelList foundByCM;
        if (allowPseudoFamily)
        {
            foundByCM = inList.findByCombinedAndManufacturer(remoteAircraft.getAircraftIcaoCode());
            const QString pseudo = remoteAircraft.getAircraftIcaoCode().getCombinedType() % "/" % remoteAircraft.getAircraftIcaoCode().getManufacturer();
            if (foundByCM.isEmpty())
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Not found by pseudo family '" % pseudo % u"' (" % hint % ")"); }
            }
            else
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found by pseudo family '" % pseudo % u"' (" % hint % u") size " % QString::number(foundByCM.sizeInt()), getLogCategories()); }
            }
        }

        if (foundByCM.isEmpty() && foundByFamily.isEmpty()) { return inList; }
        reduced = true;

        // avoid dpulicates, then add
        if (!foundByFamily.isEmpty()) { foundByCM.removeModelsWithString(foundByFamily.getModelStringList(), Qt::CaseInsensitive); }
        foundByFamily.push_back(foundByCM);

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found by family (totally) '" % family % u"' (" % hint % u") size " % QString::number(foundByFamily.sizeInt()), getLogCategories()); }
        return foundByFamily;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByManufacturer(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        const QString m = remoteAircraft.getAircraftIcaoCode().getManufacturer();
        if (m.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" No manufacturer, cannot reduce " % QString::number(inList.size()) %  u" entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByManufacturer(m));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Not found '" % m % u"', cannot reduce", getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Reduced by '" % m % u"' results: " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftIcaoCodeList CAircraftMatcher::ifPossibleReduceAircraftIcaoByManufacturer(const CAircraftIcaoCode &icaoCode, const CAircraftIcaoCodeList &inList, const QString &info, bool &reduced, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, logCallsign, info % u" Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        const QString m = icaoCode.getManufacturer();
        if (m.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, logCallsign, info % u" No manufacturer, cannot reduce " % QString::number(inList.size()) %  u" entries", getLogCategories()); }
            return inList;
        }

        const CAircraftIcaoCodeList outList(inList.findByManufacturer(m));
        if (outList.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, logCallsign, info % " Not found " % m % ", cannot reduce", getLogCategories()); }
            return inList;
        }

        if (log) { CCallsign::addLogDetailsToList(log, logCallsign, info % u" Reduced by " % m % u" results: " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAircraft(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        if (!remoteAircraft.hasAircraftDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " No aircraft designator, cannot reduce " % QString::number(inList.size()) %  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByIcaoDesignators(remoteAircraft.getAircraftIcaoCode(), CAirlineIcaoCode::null()));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Cannot reduce by '" % remoteAircraft.getAircraftIcaoCodeDesignator() % u"' results: " % QString::number(outList.size()), getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Reduced by '" % remoteAircraft.getAircraftIcaoCodeDesignator() % u"' to " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAircraftOrFamily(const CSimulatedAircraft &remoteAircraft, bool allowPseudoFamily, const CAircraftModelList &inList,  const CAircraftMatcherSetup &setup, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        const CAircraftModelList outList = ifPossibleReduceByAircraft(remoteAircraft, inList, info, reduced, log);
        if (reduced || !setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByFamily)) { return outList; }
        QString family;
        return ifPossibleReduceByFamily(remoteAircraft, allowPseudoFamily, inList, reduced, family, log);
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAirline(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const CAircraftMatcherSetup &setup, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        if (!remoteAircraft.hasAirlineDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" No airline designator, cannot reduce " % QString::number(inList.size()) %  u" entries", getLogCategories()); }
            return inList;
        }

        CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        CAircraftModelList outList(inList.findByIcaoDesignators(CAircraftIcaoCode::null(), remoteAircraft.getAirlineIcaoCode()));
        if (
            mode.testFlag(CAircraftMatcherSetup::ByAirlineGroupSameAsAirline) ||
            (outList.isEmpty() || mode.testFlag(CAircraftMatcherSetup::ByAirlineGroupIfNoAirline)))
        {
            if (remoteAircraft.getAirlineIcaoCode().hasGroupMembership())
            {
                const CAircraftModelList groupModels = inList.findByAirlineGroup(remoteAircraft.getAirlineIcaoCode());
                outList.replaceOrAddModelsWithString(groupModels, Qt::CaseInsensitive);
                if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                        groupModels.isEmpty() ?
                                                        QStringLiteral("No group models found by using airline group '%1'").arg(remoteAircraft.getAirlineIcaoCode().getGroupDesignator()) :
                                                        QStringLiteral("Added %1 model(s) by using airline group '%2', all members: '%3'").arg(groupModels.sizeInt()).arg(remoteAircraft.getAirlineIcaoCode().getGroupDesignator(), joinStringSet(groupModels.getAirlineVDesignators(), ", ")),
                                                        getLogCategories());
                }
            } // group membership
        }

        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Cannot reduce by '" % remoteAircraft.getAirlineIcaoCodeDesignator() % u"' results: " % QString::number(outList.size()), getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % u" Reduced by '" % remoteAircraft.getAirlineIcaoCodeDesignator() % u"' to " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceModelsByAirlineNameTelephonyDesignator(const CCallsign &cs, const QString &airlineName, const QString &telephony, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % u" Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        if (telephony.isEmpty() && airlineName.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % u" No name/telephony, cannot reduce " % QString::number(inList.size()) %  u" entries", getLogCategories()); }
            return inList;
        }

        CAircraftModelList step1Data = inList.findByAirlineNamesOrTelephonyDesignator(airlineName);
        if (step1Data.size() < 1 || step1Data.size() == inList.size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % QStringLiteral(" cannot reduce by '%1'").arg(airlineName), getLogCategories()); }
            step1Data = inList;
        }
        else
        {
            reduced = true;
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % QStringLiteral(" reduced by '%1'").arg(airlineName), getLogCategories()); }
        }
        if (step1Data.size() == 1) { return step1Data; }

        CAircraftModelList step2Data = inList.findByAirlineNamesOrTelephonyDesignator(telephony);
        if (step2Data.size() < 1 || step2Data.size() == inList.size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % QStringLiteral(" cannot reduce by '%1'").arg(telephony), getLogCategories()); }
            step2Data = step1Data;
        }
        else
        {
            reduced = true;
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % QStringLiteral(" reduced by '%1'").arg(telephony), getLogCategories()); }
        }
        return step2Data;

        /** alternative implementation using different finder
        const CAircraftModelList reducedModels = inList.findByAirlineNameAndTelephonyDesignator(airlineName, telephony);
        if (reducedModels.size() < 1 || reducedModels.size() == inList.size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, info % QStringLiteral(" cannot reduce by '%1'/'%2'").arg(airlineName, telephony), getLogCategories()); }
            return inList;
        }

        reduced = true;
        return reducedModels;
        **/
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByCombinedType(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const CAircraftMatcherSetup &setup, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getAircraftIcaoCode().hasValidCombinedType())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No valid combined code"), getLogCategories()); }
            return inList;
        }

        const QString cc = remoteAircraft.getAircraftIcaoCode().getCombinedType();
        CAircraftModelList modelsByCombinedCode(inList.findByCombinedType(cc));
        if (modelsByCombinedCode.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Not found by combined code " % cc, getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Found by combined code " % cc % u", possible " % QString::number(modelsByCombinedCode.size()), getLogCategories()); }
        if (modelsByCombinedCode.size() > 1)
        {
            modelsByCombinedCode = ifPossibleReduceByAirline(remoteAircraft, modelsByCombinedCode, setup, QStringLiteral("Combined code airline reduction. "), reduced, log);
            modelsByCombinedCode = ifPossibleReduceByManufacturer(remoteAircraft, modelsByCombinedCode, QStringLiteral("Combined code manufacturer reduction. "), reduced, log);
            reduced = true;
        }
        return modelsByCombinedCode;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByMilitaryFlag(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        const bool military = remoteAircraft.getModel().isMilitary();
        const CAircraftModelList byMilitaryFlag(inList.findByMilitaryFlag(military));
        const QString mil(military ? "military" : "civilian");
        if (byMilitaryFlag.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Models not found by " % mil, getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Models reduced to " % mil % u" aircraft, size " % QString::number(byMilitaryFlag.size()), getLogCategories()); }
        return byMilitaryFlag;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByVTOLFlag(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!inList.containsVtol())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Cannot reduce to VTOL aircraft", getLogCategories());
            return inList;
        }
        CAircraftModelList vtolModels = inList.findByVtolFlag(true);
        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, u"Models reduced to " % QString::number(vtolModels.size()) % u" VTOL aircraft", getLogCategories()); }
        return vtolModels;
    }

    QString CAircraftMatcher::scoresToString(const ScoredModels &scores, int lastElements)
    {
        if (scores.isEmpty()) { return {}; }
        QMapIterator<int, CAircraftModel> i(scores);
        i.toBack();
        int c = 0;
        QString str;
        while (i.hasPrevious() && c++ < lastElements)
        {
            i.previous();
            const CAircraftModel m(i.value());
            if (!str.isEmpty()) { str += '\n'; }
            str += QString::number(c) %
                   u": score: " %
                   QString::number(i.key()) %
                   u" model: '" %
                   m.getModelString() %
                   u"' aircraft: '" %
                   m.getAircraftIcaoCodeDesignator() %
                   u"' livery: '" %
                   m.getLivery().getCombinedCodePlusInfo() %
                   u'\'';
        }
        return str;
    }

    CAirlineIcaoCode CAircraftMatcher::stringToAirlineIcaoObject(const CCallsign &cs, const QString &designator, const QString &airlineName, const QString &airlineTelephony, bool useSwiftDbData, CStatusMessageList *log)
    {
        if (!useSwiftDbData) { return CAirlineIcaoCode(designator); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirlineIcaoCode(designator); }
        const CAirlineIcaoCodeList codes = sApp->getWebDataServices()->getAirlineIcaoCodesForDesignator(designator);
        if (codes.isEmpty())   { return CAirlineIcaoCode(designator); }
        if (codes.size() == 1) { return codes.front(); }

        // more than 1
        bool reduced = false;
        static const QString info("Try reducing airline '%1' by name/telephony '%2'/'%3'");
        const CAirlineIcaoCodeList reducedIcaos = codes.ifPossibleReduceNameTelephonyCountry(cs, airlineName, airlineTelephony, QString(), reduced, info, log);
        return reducedIcaos.frontOrDefault();
    }

    bool CAircraftMatcher::isValidAirlineIcaoDesignator(const QString &designator, bool checkAgainstSwiftDb)
    {
        if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return false; }
        if (!checkAgainstSwiftDb) { return true; }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return true; }
        return (sApp->getWebDataServices()->containsAirlineIcaoDesignator(designator));
    }
} // namespace
