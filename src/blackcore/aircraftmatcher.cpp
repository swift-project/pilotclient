/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/aircraftmatcher.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessagelist.h"

#include <QList>
#include <QStringList>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    const CLogCategoryList &CAircraftMatcher::getLogCategories()
    {
        static const CLogCategoryList cats { CLogCategory::matching() };
        return cats;
    }

    CAircraftMatcher::CAircraftMatcher(const CAircraftMatcherSetup &setup, QObject *parent) :
        QObject(parent),
        m_setup(setup)
    { }

    CAircraftMatcher::CAircraftMatcher(QObject *parent) : CAircraftMatcher(CAircraftMatcherSetup(), parent)
    { }

    CAircraftMatcher::~CAircraftMatcher()
    { }

    bool CAircraftMatcher::setSetup(const CAircraftMatcherSetup &setup)
    {
        if (m_setup == setup) { return false; }
        m_setup = setup;
        emit this->setupChanged();
        return true;
    }

    CAirlineIcaoCode CAircraftMatcher::failoverValidAirlineIcaoDesignator(
        const CCallsign &callsign, const QString &primaryIcao, const QString &secondaryIcao,
        bool airlineFromCallsign, bool useWebServices, CStatusMessageList *log)
    {
        CMatchingUtils::addLogDetailsToList(log, callsign,
                                            QString("Find airline designator from 1st: '%1' 2nd: '%2' callsign: '%3', use airline callsign: %4, use web service: %5").
                                            arg(primaryIcao, secondaryIcao, callsign.getAirlineSuffix(), boolToYesNo(airlineFromCallsign), boolToYesNo(useWebServices)), getLogCategories());
        CAirlineIcaoCode code;
        do
        {
            if (CAircraftMatcher::isValidAirlineIcaoDesignator(primaryIcao, useWebServices))
            {
                code = stringToAirlineIcaoObject(primaryIcao, useWebServices);
                break;
            }
            if (CAircraftMatcher::isValidAirlineIcaoDesignator(secondaryIcao, useWebServices))
            {
                CMatchingUtils::addLogDetailsToList(log, callsign, QString("Using secondary airline ICAO '%1', primary '%2' not valid").arg(secondaryIcao, primaryIcao), getLogCategories());
                code = stringToAirlineIcaoObject(secondaryIcao, useWebServices);
                break;
            }

            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Two invalid airline ICAO codes (primary/secondary) '%1', '%2'").arg(primaryIcao, secondaryIcao), getLogCategories());
            if (airlineFromCallsign)
            {
                const QString airlineSuffix = callsign.getAirlineSuffix();
                if (CAircraftMatcher::isValidAirlineIcaoDesignator(airlineSuffix, useWebServices))
                {
                    CMatchingUtils::addLogDetailsToList(log, callsign, QString("Using airline from callsign '%1', suffix: '%2'").arg(callsign.toQString(), airlineSuffix), getLogCategories());
                    code = stringToAirlineIcaoObject(airlineSuffix, useWebServices);
                    break;
                }
            }
        }
        while (false);
        CMatchingUtils::addLogDetailsToList(log, callsign, QString("Resolved to airline designator: %1").arg(code.toQString(true)));
        return code;
    }

    CAircraftModel CAircraftMatcher::getClosestMatch(const CSimulatedAircraft &remoteAircraft, CStatusMessageList *log) const
    {
        CAircraftModelList modelSet(m_modelSet); // Models for this matching
        const CAircraftMatcherSetup setup = m_setup;

        static const QString format("hh:mm:ss.zzz");
        static const QString m1("--- Start matching: UTC %1 ---");
        static const QString m2("Input model: '%1' '%2'");
        static const QString m3("Matching uses model set of %1 models\n%2");
        static const QString m4("Setup %1");

        const QDateTime startTime = QDateTime::currentDateTimeUtc();
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m1.arg(startTime.toString(format)));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m2.arg(remoteAircraft.getCallsignAsString(), remoteAircraft.getModel().toQString()));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, m3.arg(modelSet.size()).arg(modelSet.extCoverageSummary(remoteAircraft.getModel())));
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
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Manually set model ") % remoteAircraft.getModelString(), getLogCategories());
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
                matchedModel = matchByExactModelString(remoteAircraft, modelSet, log);
                if (matchedModel.hasModelString())
                {
                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Exact match by model string '") % matchedModel.getModelStringAndDbKey() % QStringLiteral("'"), getLogCategories(), CStatusMessage::SeverityError);
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
            // Reduce by ICAO if the flag is set
            static const QString msInfo("Using '%1' with model set with %2 models");
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, msInfo.arg(setup.getMatchingAlgorithmAsString()).arg(modelSet.size()), getLogCategories());
            CAircraftModelList candidates;
            int maxScore = -1;

            switch (setup.getMatchingAlgorithm())
            {
            case CAircraftMatcherSetup::MatchingStepwiseReduce:
                candidates = CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(modelSet, setup, remoteAircraft, log);
                break;
            case CAircraftMatcherSetup::MatchingScoreBased:
            default:
                candidates = CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(modelSet, setup, remoteAircraft, log);
                candidates = CAircraftMatcher::getClosestMatchScoreImplementation(candidates, setup, remoteAircraft, maxScore, log);
                break;
            }

            if (candidates.isEmpty())
            {
                matchedModel = CAircraftMatcher::getCombinedTypeDefaultModel(modelSet, remoteAircraft, this->getDefaultModel(), log);
            }
            else
            {
                static const QString ps("Picking among %1 by strategy '%2'");
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
                // [[clang::fallthrough]];
                case CAircraftMatcherSetup::PickFirst:
                default:
                    usedStrategy = CAircraftMatcherSetup::PickFirst; // re-assigned if fall-through
                    matchedModel = candidates.front();
                    break;
                }
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, ps.arg(candidates.size()).arg(CAircraftMatcherSetup::strategyToString(usedStrategy)));
            }
        }

        // copy over callsign validate
        matchedModel.setCallsign(remoteAircraft.getCallsign());

        Q_ASSERT_X(!matchedModel.getCallsign().isEmpty(), Q_FUNC_INFO, "Missing callsign");
        Q_ASSERT_X(matchedModel.hasModelString(), Q_FUNC_INFO, "Missing model string");
        Q_ASSERT_X(matchedModel.getModelType() != CAircraftModel::TypeUnknown, Q_FUNC_INFO, "Missing model type");

        const QDateTime endTime = QDateTime::currentDateTimeUtc();
        const qint64 matchingTime = startTime.msecsTo(endTime);
        static const QString em("--- Matching end: UTC %1, time %2ms ---");
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, em.arg(endTime.toString(format)).arg(matchingTime));
        return matchedModel;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModel(const CCallsign &callsign, const CAircraftIcaoCode &networkAircraftIcao, const CAirlineIcaoCode &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString, CAircraftModel::ModelType type, CStatusMessageList *log)
    {
        CLivery livery;
        livery.setAirlineIcaoCode(networkAirlineIcao);
        CAircraftModel model(networkModelString, type, "", networkAircraftIcao, livery);
        model.setCallsign(callsign);
        model = CAircraftMatcher::reverseLookupModel(model, networkLiveryInfo, log);
        return model;
    }

    CAircraftModel CAircraftMatcher::reverseLookupModel(const CAircraftModel &modelToLookup, const QString &networkLiveryInfo, CStatusMessageList *log)
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
                if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup found DB model '%1' for '%2'").arg(modelFromDb.getModelStringAndDbKey(), callsign.toQString()), getLogCategories()); }
            }
            else
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup did not find model for '%1'").arg(modelString), getLogCategories()); }
            }
        }

        // lookup if model is not yet from DB
        if (!model.isLoadedFromDb())
        {
            // check if livery is already from DB
            if (!model.getLivery().isLoadedFromDb())
            {
                CAirlineIcaoCode airlineIcaoCode(model.getAirlineIcaoCode());
                if (!airlineIcaoCode.isLoadedFromDb())
                {
                    airlineIcaoCode = CAircraftMatcher::reverseLookupAirlineIcao(airlineIcaoCode, callsign, log);
                }

                // try to match by livery
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
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of livery found '%1'").arg(reverseLivery.getCombinedCodePlusInfoAndId()), getLogCategories()); }
                    }
                    else
                    {
                        // no livery data found
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of livery '%1' yielded no result").arg(reverseLivery.getCombinedCodePlusInfo()), getLogCategories()); }
                    }
                } // livery lookup

                // if no DB livery yet, create own livery
                if (!model.hasValidDbKey() && !model.getLivery().hasValidDbKey())
                {
                    // create a pseudo livery, try to find airline first
                    if (airlineIcaoCode.hasValidDesignator())
                    {
                        const CLivery liveryDummy(CLivery::getStandardCode(airlineIcaoCode), airlineIcaoCode, "Generated");
                        model.setLivery(liveryDummy);
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Generated livery, set livery `%1`").arg(liveryDummy.getCombinedCodePlusInfo())); }
                    }
                } // pseudo livery
            } // livery from DB

            // aircraft ICAO if not from DB yet
            if (!model.getAircraftIcaoCode().hasValidDbKey())
            {
                CAircraftIcaoCode reverseIcaoCode(model.getAircraftIcaoCode());
                if (!reverseIcaoCode.isLoadedFromDb())
                {
                    reverseIcaoCode = CAircraftMatcher::reverseLookupAircraftIcao(reverseIcaoCode, callsign, log);
                    if (reverseIcaoCode.isLoadedFromDb())
                    {
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Set aircraft ICAO to '%1' from DB").arg(reverseIcaoCode.getCombinedIcaoStringWithKey())); }
                        model.setAircraftIcaoCode(reverseIcaoCode);
                    }
                    else
                    {
                        // no DB data
                        if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup, ICAO '%1' not resolved from DB").arg(reverseIcaoCode.getDesignator())); }
                    }
                }
            }
        } // model from DB

        model.setCallsign(callsign);
        model.setModelType(modelToLookup.getModelType());

        if (log)
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Using model: ICAO '%1', livery '%2', model '%3', type '%4'").arg(
                                                    model.getAircraftIcaoCode().getCombinedIcaoStringWithKey(),
                                                    model.getLivery().getCombinedCodePlusInfo(),
                                                    model.getModelString(), model.getModelTypeAsString()));
        }
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
                CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of aircraft ICAO '%1' did not find anything, using smart search").arg(designator), CAircraftMatcher::getLogCategories());
                icao = sApp->getWebDataServices()->smartAircraftIcaoSelector(icaoCandidate);
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of invalid ICAO code '%1' did not find anything so far").arg(designator), CAircraftMatcher::getLogCategories());
                const QStringList parts(designator.split(' '));
                for (const QString &p : parts)
                {
                    CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Trying parts, now reverse lookup of aircraft ICAO '%1' using smart search").arg(p), CAircraftMatcher::getLogCategories());
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
                CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("No DB data for ICAO '%1', valid ICAO?").arg(designator), CAircraftMatcher::getLogCategories());
                return CAircraftIcaoCode(icaoCandidate);
            }
        }

        if (foundIcaos.size() < 1)
        {
            CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of aircraft ICAO '%1'', nothing found").arg(designator), CAircraftMatcher::getLogCategories());
            return CAircraftIcaoCode(icaoCandidate);
        }
        else if (foundIcaos.size() == 1)
        {
            const CAircraftIcaoCode icao(foundIcaos.front());
            CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of aircraft ICAO '%1'', found one manufacturer '%2' in DB").arg(designator, icao.getDesignatorManufacturer()), CAircraftMatcher::getLogCategories());
            return icao;
        }
        else
        {
            // multiple ICAOs
            Q_ASSERT_X(foundIcaos.size() > 1, Q_FUNC_INFO, "Wrong size");
            const QPair<QString, int> maxManufacturer = foundIcaos.maxCountManufacturer();
            CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of aircraft ICAO '%1'', found %2 values (ambiguous): %3").arg(designator).arg(foundIcaos.size()).arg(foundIcaos.dbKeysAsString(", ")), CAircraftMatcher::getLogCategories());
            if (maxManufacturer.second < foundIcaos.size())
            {
                foundIcaos = foundIcaos.findByManufacturer(maxManufacturer.first);
                CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reducing by manufacturer '%1', now %2 values").arg(maxManufacturer.first).arg(foundIcaos.size()), CAircraftMatcher::getLogCategories());
            }
            foundIcaos.sortByRank();
            const CAircraftIcaoCode icao = foundIcaos.front(); // best rank
            CMatchingUtils::addLogDetailsToList(log, logCallsign, QString("Reverse lookup of aircraft ICAO '%1'', using ICAO '%2' with rank %3").arg(designator, icao.toQString(), icao.getRankString()), CAircraftMatcher::getLogCategories());
            return icao;
        }
    }

    CAirlineIcaoCode CAircraftMatcher::reverseLookupAirlineIcao(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode icao = sApp->getWebDataServices()->smartAirlineIcaoSelector(icaoPattern, callsign);
        if (log)
        {
            if (icao.hasValidDbKey()) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of airline ICAO '%1' and callsign '%2' found '%3' '%4' in DB").arg(icaoPattern.getDesignator(), callsign.asString(), icao.getVDesignatorDbKey(), icao.getName()), CAircraftMatcher::getLogCategories()); }
            else { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of airline ICAO '%1' and callsign '%2', nothing found in DB").arg(icaoPattern.getDesignator(), callsign.asString()), CAircraftMatcher::getLogCategories()); }
        }
        return icao;
    }

    CLivery CAircraftMatcher::reverseLookupStandardLivery(const CAirlineIcaoCode &airline, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CLivery(); }
        if (!airline.hasValidDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, callsign, "Reverse lookup of standard livery skipped, no airline designator", CAircraftMatcher::getLogCategories(), CStatusMessage::SeverityWarning); }
            return CLivery();
        }

        const CLivery livery = sApp->getWebDataServices()->getStdLiveryForAirlineCode(airline);
        if (log)
        {
            if (livery.hasValidDbKey()) { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Reverse lookup of standard livery for '%1' found '%2'").arg(airline.getDesignator(), livery.getCombinedCode()), CAircraftMatcher::getLogCategories()); }
            else { CMatchingUtils::addLogDetailsToList(log, callsign, QString("Not standard livery for airline '%1' in DB").arg(airline.getDesignator()), CAircraftMatcher::getLogCategories()); }
        }
        return livery;
    }

    QString CAircraftMatcher::reverseLookupAirlineName(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return ""; }
        if (candidate.isEmpty()) { return ""; }
        const QStringList names = sApp->getWebDataServices()->getAirlineNames();
        if (names.contains(candidate, Qt::CaseInsensitive))
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Airline name '%1' found in DB").arg(candidate));
            return candidate;
        }

        CMatchingUtils::addLogDetailsToList(log, callsign, QString("Airline name '%1' not found in DB").arg(candidate));
        return "";
    }

    QString CAircraftMatcher::reverseLookupTelephonyDesignator(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return ""; }
        if (candidate.isEmpty()) { return ""; }
        const QStringList designators = sApp->getWebDataServices()->getTelephonyDesignators();
        if (designators.contains(candidate, Qt::CaseInsensitive))
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Airline name '%1' found").arg(candidate));
            return candidate;
        }

        CMatchingUtils::addLogDetailsToList(log, callsign, QString("Airline name '%1' not found").arg(candidate));
        return "";
    }

    bool CAircraftMatcher::isKnowAircraftDesignator(const QString &candidate, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return false; }
        if (!CAircraftIcaoCode::isValidDesignator(candidate))
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("No valid ICAO designator '%1'").arg(candidate));
            return false;
        }

        const bool known = sApp->getWebDataServices()->containsAircraftIcaoDesignator(candidate);
        static const QString sKnown("Known ICAO '%1'");
        static const QString sUnknown("Unknown ICAO '%1'");
        CMatchingUtils::addLogDetailsToList(log, callsign, known ? sKnown.arg(candidate) : sUnknown.arg(candidate));
        return known;
    }

    CAircraftIcaoCode CAircraftMatcher::searchAmongAirlineAircraft(const QString &candidateString, const CAirlineIcaoCode &airline, const CCallsign &callsign, CStatusMessageList *log)
    {
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAircraftIcaoCode(); }
        if (!airline.isLoadedFromDb())
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("No valid airline from DB '%1'").arg(airline.getDesignator()));
            return CAircraftIcaoCode();
        }

        Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "No web services");

        const CAircraftIcaoCodeList aircraft = sApp->getWebDataServices()->getAircraftIcaoCodesForAirline(airline);
        if (aircraft.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("No aircraft known for airline '%1'").arg(airline.getDesignator()));
            return CAircraftIcaoCode();
        }

        const QSet<QString> allIcaos = aircraft.allIcaoCodes();
        const QString allIcaosStr = allIcaos.toList().join(", ");
        CMatchingUtils::addLogDetailsToList(log, callsign, QString("Aircraft '%1' known for airline '%2'").arg(allIcaosStr, airline.getDesignator()));

        const CAircraftIcaoCode code = aircraft.findBestFuzzyMatchOrDefault(candidateString);
        if (code.hasValidDesignator())
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Aircraft '%1' is best fuzzy search of '%2' for airline '%3'").arg(code.toQString(), candidateString, airline.getDesignator()));
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
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Turned callsign %1 into airline %2").arg(callsign.asString(), icao.getDesignator()), getLogCategories());
        }
        else
        {
            CMatchingUtils::addLogDetailsToList(log, callsign, QString("Cannot turn callsign %1 into airline").arg(callsign.asString()), getLogCategories());
        }
        return icao;
    }

    int CAircraftMatcher::setModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator, bool forced)
    {
        if (!simulator.isSingleSimulator()) { return 0; }
        if (!forced && m_simulator == simulator && m_modelSet.size() > 0)
        {
            // same simulator with models
            return m_modelSet.size();
        }

        CAircraftModelList modelsCleaned(models);
        const int r1 = modelsCleaned.removeAllWithoutModelString();
        const int r2 = modelsCleaned.removeIfExcluded();
        if ((r1 + r2) > 0)
        {
            CLogMessage(this).warning("Removed models for matcher, without string #: %1, excluded #: %2") << r1 << r2;
            if (r1 > 0) { CLogMessage(this).warning("Without string: %1") << models.findEmptyModelStrings().getModelStringList().join(", "); }
            if (r2 > 0) { CLogMessage(this).warning("Excluded: %1") << models.findByModelMode(CAircraftModel::Exclude).getModelStringList().join(", "); }
        }
        if (modelsCleaned.isEmpty())
        {
            CLogMessage(this).error("No models for matching ('%1'), swift without a model set will not work!") << simulator.toQString();
        }
        else
        {
            CLogMessage(this).info("Set %1 models in matcher, simulator '%2'") << modelsCleaned.size() << simulator.toQString();
        }
        m_modelSet = modelsCleaned;
        m_simulator = simulator;
        m_modelSetInfo = QString("Set: '%1' entries: %2").arg(simulator.toQString()).arg(modelsCleaned.size());
        return models.size();
    }

    void CAircraftMatcher::setDefaultModel(const CAircraftModel &defaultModel)
    {
        m_defaultModel = defaultModel;
        m_defaultModel.setModelType(CAircraftModel::TypeModelMatchingDefaultModel);
    }

    void CAircraftMatcher::evaluateStatisticsEntry(const QString &sessionId, const CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
    {
        Q_UNUSED(livery);
        Q_ASSERT_X(sApp && sApp->hasWebDataServices(), Q_FUNC_INFO, "Missing web data services");
        if (m_modelSet.isEmpty()) { return; } // ignore empty sets to not create silly stats
        if (sessionId.isEmpty()) { return; }
        if (aircraftIcao.isEmpty()) { return; }

        QString description;
        if (!sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator(aircraftIcao).isLoadedFromDb())
        {
            static const QString d("ICAO: '%1' not known, typo?");
            description = d.arg(aircraftIcao);
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

    CAircraftModelList CAircraftMatcher::getClosestMatchStepwiseReduceImplementation(const CAircraftModelList &modelSet, const CAircraftMatcherSetup &setup, const CSimulatedAircraft &remoteAircraft, CStatusMessageList *log)
    {
        CAircraftModelList matchedModels(modelSet);
        CAircraftModel matchedModel(remoteAircraft.getModel());

        const CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        bool reduced = false;
        do
        {
            // by livery, then by ICAO
            if (mode.testFlag(CAircraftMatcherSetup::ByLivery))
            {
                matchedModels = ifPossibleReduceByLiveryAndAircraftIcaoCode(remoteAircraft, matchedModels, reduced, log);
                if (reduced) { break; } // almost perfect, we stop here
            }
            else if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping livery reduction", getLogCategories());
            }

            if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByIcaoData))
            {
                matchedModels = ifPossibleReduceByIcaoData(remoteAircraft, matchedModels, setup, reduced, log);
            }
            else if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Skipping ICAO reduction"), getLogCategories());
            }

            // if not yet reduced, reduce to VTOL
            if (!reduced && remoteAircraft.isVtol() && matchedModels.containsVtol())
            {
                matchedModels = matchedModels.findByVtolFlag(true);
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Aircraft is VTOL, reduced to VTOL", getLogCategories());
            }

            // family
            if (mode.testFlag(CAircraftMatcherSetup::ByFamily))
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
            if (mode.testFlag(CAircraftMatcherSetup::ByCombinedType))
            {
                matchedModels = ifPossibleReduceByCombinedCode(remoteAircraft, matchedModels, reduced, log);
                if (reduced) { break; }
            }
            else if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Skipping combined code match", getLogCategories());
            }
        }
        while (false);

        // here we have a list of possible models, we reduce/refine further
        if (matchedModels.size() > 1)
        {
            matchedModels = ifPossibleReduceByManufacturer(remoteAircraft, matchedModels, "2nd pass", reduced, log);
            matchedModels = ifPossibleReduceByMilitaryFlag(remoteAircraft, matchedModels, reduced, log);
        }

        return matchedModels;
    }

    CAircraftModelList CAircraftMatcher::getClosestMatchScoreImplementation(const CAircraftModelList &modelSet, const CAircraftMatcherSetup &setup, const CSimulatedAircraft &remoteAircraft, int &maxScore, CStatusMessageList *log)
    {
        CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        const bool noZeroScores = mode.testFlag(CAircraftMatcherSetup::ScoreIgnoreZeros);
        const bool preferColorLiveries = mode.testFlag(CAircraftMatcherSetup::ScorePreferColorLiveries);

        // VTOL
        ScoredModels map;
        map = modelSet.scoreFull(remoteAircraft.getModel(), preferColorLiveries, noZeroScores, log);

        CAircraftModel matchedModel;
        if (map.isEmpty()) { return CAircraftModelList(); }

        maxScore = map.lastKey();
        const CAircraftModelList maxScoreAircraft(map.values(maxScore));
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QString("Scores: %1").arg(scoresToString(map)), getLogCategories());
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QString("Scoring with score %1 out of %2 models yielded %3 models").arg(maxScore).arg(map.size()).arg(maxScoreAircraft.size()), getLogCategories());
        return maxScoreAircraft;
    }

    CAircraftModel CAircraftMatcher::getCombinedTypeDefaultModel(const CAircraftModelList &modelSet, const CSimulatedAircraft &remoteAircraft, const CAircraftModel &defaultModel, CStatusMessageList *log)
    {
        const QString combinedType = remoteAircraft.getAircraftIcaoCombinedType();
        if (combinedType.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No combined type, using default", getLogCategories(), CStatusMessage::SeverityInfo);
            return defaultModel;
        }
        if (modelSet.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No models, using default", getLogCategories(), CStatusMessage::SeverityError);
            return defaultModel;
        }

        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Searching by combined type with color livery '" % combinedType % "'", getLogCategories());
        CAircraftModelList matchedModels = modelSet.findByCombinedTypeWithColorLivery(combinedType);
        if (!matchedModels.isEmpty())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found " % QString::number(matchedModels.size()) % " by combined type w/color livery '" % combinedType % "'", getLogCategories());
        }
        else
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Searching by combined type '" % combinedType % "'", getLogCategories());
            matchedModels = matchedModels.findByCombinedType(combinedType);
            if (!matchedModels.isEmpty())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found " % QString::number(matchedModels.size()) % " by combined '" % combinedType % "'", getLogCategories());
            }
        }

        // here we have a list of possible models, we reduce/refine further
        if (matchedModels.size() > 1)
        {
            bool reduced = false;
            matchedModels = ifPossibleReduceByManufacturer(remoteAircraft, matchedModels, "combined code reduction", reduced, log);
            matchedModels = ifPossibleReduceByMilitaryFlag(remoteAircraft, matchedModels, reduced, log);
        }

        // return
        if (matchedModels.isEmpty()) { return defaultModel; }
        return matchedModels.front();
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
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QString("Found exact match for '%1'").arg(model.getModelString()));
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QString("No exact match for '%1'").arg(remoteAircraft.getModelString()));
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by livery code " % remoteAircraft.getLivery().getCombinedCode(), getLogCategories()); }
            return inList;
        }
        reduced = true;
        return byLivery;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByIcaoData(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const CAircraftMatcherSetup &setup, bool &reduced, CStatusMessageList *log)
    {
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Empty list, skipping step", getLogCategories()); }
            return inList;
        }

        reduced = false;
        if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByIcaoOrderAirlineFirst))
        {
            bool r;
            CAircraftModelList models = ifPossibleReduceByAirline(remoteAircraft, inList, "Reduce by airline first.", r, log);
            models = ifPossibleReduceByAircraft(remoteAircraft, models, "Reduce by aircraft ICAO second.", r, log);
            reduced = r;
            if (reduced) { return models; }
        }
        else if (setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByIcaoData))
        {
            bool r;
            CAircraftModelList models = ifPossibleReduceByAircraft(remoteAircraft, inList, "Reduce by aircraft ICAO first.", r, log);
            models = ifPossibleReduceByAirline(remoteAircraft, models, "Reduce by airline second.", r, log);
            reduced = r;
            if (reduced) { return models; }
        }
        else
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No reduction by ICAO data", getLogCategories()); }
        }

        return inList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByFamily(const CSimulatedAircraft &remoteAircraft, const QString &family, const CAircraftModelList &inList, const QString &hint, bool &reduced, CStatusMessageList *log)
    {
        // Use an algorithm to find the best match
        reduced = true;
        if (family.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No family, skipping step (" % hint % ")", getLogCategories()); }
            return inList;
        }
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No models for family match (" % hint % ")", getLogCategories()); }
            return inList;
        }

        CAircraftModelList found(inList.findByFamily(family));
        if (found.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by family " % family % " (" % hint % ")"); }
            return inList;
        }

        reduced = true;
        CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found by family " % family % " (" % hint % ") size " % QString::number(found.size()), getLogCategories());
        return found;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByManufacturer(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " " % "Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        const QString m = remoteAircraft.getAircraftIcaoCode().getManufacturer();
        if (m.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " No manufacturer, cannot reduce " % QString::number(inList.size()) %  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByManufacturer(m));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Not found " % m % ", cannot reduce", getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Reduced by " % m % " results: " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftIcaoCodeList CAircraftMatcher::ifPossibleReduceAircraftIcaoByManufacturer(const CAircraftIcaoCode &icaoCode, const CAircraftIcaoCodeList &inList, const QString &info, bool &reduced, const CCallsign &logCallsign, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, logCallsign, info % " Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        const QString m = icaoCode.getManufacturer();
        if (m.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, logCallsign, info % " No manufacturer, cannot reduce " % QString::number(inList.size()) %  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftIcaoCodeList outList(inList.findByManufacturer(m));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, logCallsign, info % " Not found " % m % ", cannot reduce", getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, logCallsign, info % " Reduced by " % m % " results: " % QString::number(outList.size()), getLogCategories()); }
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
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Cannot reduce by '" % remoteAircraft.getAircraftIcaoCodeDesignator() % "' results: " % QString::number(outList.size()), getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Reduced reduce by '" % remoteAircraft.getAircraftIcaoCodeDesignator() % "' to " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByAirline(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, const QString &info, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (inList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Empty input list, cannot reduce", getLogCategories()); }
            return inList;
        }

        if (!remoteAircraft.hasAirlineDesignator())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " No airline designator, cannot reduce " % QString::number(inList.size()) %  " entries", getLogCategories()); }
            return inList;
        }

        const CAircraftModelList outList(inList.findByIcaoDesignators(CAircraftIcaoCode::null(), remoteAircraft.getAirlineIcaoCode()));
        if (outList.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Cannot reduce by '" % remoteAircraft.getAirlineIcaoCodeDesignator() % "' results: " % QString::number(outList.size()), getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, info % " Reduced reduce by '" % remoteAircraft.getAirlineIcaoCodeDesignator() % "' to " % QString::number(outList.size()), getLogCategories()); }
        reduced = true;
        return outList;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByCombinedCode(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        if (!remoteAircraft.getAircraftIcaoCode().hasValidCombinedType())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "No valid combined code", getLogCategories()); }
            return inList;
        }

        const QString cc = remoteAircraft.getAircraftIcaoCode().getCombinedType();
        CAircraftModelList byCombinedCode(inList.findByCombinedType(cc));
        if (byCombinedCode.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Not found by combined code " % cc, getLogCategories()); }
            return inList;
        }

        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Found by combined code " % cc % ", possible " % QString::number(byCombinedCode.size()), getLogCategories()); }
        if (byCombinedCode.size() > 1)
        {
            byCombinedCode = ifPossibleReduceByAirline(remoteAircraft, byCombinedCode, "Combined code", reduced, log);
            byCombinedCode = ifPossibleReduceByManufacturer(remoteAircraft, byCombinedCode, "Combined code", reduced, log);
            reduced = true;
        }
        return byCombinedCode;
    }

    CAircraftModelList CAircraftMatcher::ifPossibleReduceByMilitaryFlag(const CSimulatedAircraft &remoteAircraft, const CAircraftModelList &inList, bool &reduced, CStatusMessageList *log)
    {
        reduced = false;
        const bool military = remoteAircraft.getModel().isMilitary();
        const CAircraftModelList byMilitaryFlag(inList.findByMilitaryFlag(military));
        const QString mil(military ? "military" : "civilian");
        if (byMilitaryFlag.isEmpty())
        {
            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Models not found by " % mil, getLogCategories()); }
            return inList;
        }

        if (log)
        {
            if (inList.size() > byMilitaryFlag.size())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Models reduced to " % mil % " aircraft, size " % QString::number(byMilitaryFlag.size()), getLogCategories());
            }
            else
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, "Models not reduced by " % mil % ", size " % QString::number(byMilitaryFlag.size()), getLogCategories());
            }
        }
        return byMilitaryFlag;
    }

    QString CAircraftMatcher::scoresToString(const ScoredModels &scores, int lastElements)
    {
        if (scores.isEmpty()) { return ""; }
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
                   QStringLiteral(": score: ") %
                   QString::number(i.key()) %
                   QStringLiteral(" model: '") %
                   m.getModelString() %
                   QStringLiteral("' aircraft: '") %
                   m.getAircraftIcaoCodeDesignator() %
                   QStringLiteral("' livery: '") %
                   m.getLivery().getCombinedCodePlusInfo() %
                   QLatin1Char('\'');
        }
        return str;
    }

    CAirlineIcaoCode CAircraftMatcher::stringToAirlineIcaoObject(const QString &designator, bool useSwiftDbData)
    {
        if (!useSwiftDbData) { return CAirlineIcaoCode(designator); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirlineIcaoCode(designator); }
        CAirlineIcaoCode code = sApp->getWebDataServices()->getAirlineIcaoCodeForUniqueDesignatorOrDefault(designator, true);
        return code.isLoadedFromDb() ? code : CAirlineIcaoCode(designator);
    }

    bool CAircraftMatcher::isValidAirlineIcaoDesignator(const QString &designator, bool checkAgainstSwiftDb)
    {
        if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return false; }
        if (!checkAgainstSwiftDb) { return true; }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return true; }
        return (sApp->getWebDataServices()->containsAirlineIcaoDesignator(designator));
    }
} // namespace
