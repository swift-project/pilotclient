// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolator.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolatorlinear.h"
#include "blackmisc/simulation/interpolatorspline.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackmisc/stringutils.h"
#include <QTimer>
#include <QDateTime>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Simulation
{
    template <typename Derived>
    CInterpolator<Derived>::CInterpolator(const CCallsign &callsign,
                                          ISimulationEnvironmentProvider *simEnvProvider,
                                          IInterpolationSetupProvider *setupProvider,
                                          IRemoteAircraftProvider *remoteProvider,
                                          CInterpolationLogger *logger) : m_callsign(callsign)
    {
        // normally when created m_cg is still null since there is no CG in the provider yet

        if (simEnvProvider) { this->setSimulationEnvironmentProvider(simEnvProvider); }
        if (setupProvider) { this->setInterpolationSetupProvider(setupProvider); }
        if (remoteProvider)
        {
            this->setRemoteAircraftProvider(remoteProvider);
            QObject::connect(&m_initTimer, &QTimer::timeout, [=] { this->deferredInit(); });
            m_initTimer.setSingleShot(true);
            m_initTimer.start(2500);
        }
        this->attachLogger(logger);
    }

    template <typename Derived>
    CLength CInterpolator<Derived>::getAndFetchModelCG(const CLength &dbCG)
    {
        CLength cgDb = dbCG;
        if (cgDb.isNull())
        {
            // no input DB value, try to find one
            cgDb = this->getRemoteAircraftProvider() ? this->getRemoteAircraftProvider()->getCGFromDB(m_callsign) : CLength::null();
        }
        else if (this->getRemoteAircraftProvider())
        {
            // if a value has been passed, remember it
            if (m_model.hasModelString()) { this->getRemoteAircraftProvider()->rememberCGFromDB(cgDb, m_model.getModelString()); }
            if (!m_model.getCallsign().isEmpty()) { this->getRemoteAircraftProvider()->rememberCGFromDB(cgDb, m_model.getCallsign()); }
        }

        const CLength cg = this->getSimulatorOrDbCG(m_callsign, cgDb); // simulation environment
        m_model.setCG(cg);
        m_model.setCallsign(m_callsign);
        return cg;
    }

    template <typename Derived>
    double CInterpolator<Derived>::groundInterpolationFactor()
    {
        // done here so we can change value without "larfer" recompilations
        static constexpr double f = 0.95;
        return f;
    }

    template <typename Derived>
    CAircraftSituationList CInterpolator<Derived>::remoteAircraftSituationsAndChange(const CInterpolationAndRenderingSetupPerCallsign &setup)
    {
        CAircraftSituationList validSituations = this->remoteAircraftSituations(m_callsign);

        // get the changes, we need the second value as we want to look in the past
        // the first value is already based on the latest situation
        const CAircraftSituationChangeList changes = this->remoteAircraftSituationChanges(m_callsign);
        m_pastSituationsChange = changes.indexOrNull(1);

        // fixing offset
        if (setup.isFixingSceneryOffset() && m_pastSituationsChange.hasSceneryDeviation() && m_model.hasCG())
        {
            const CLength os = m_pastSituationsChange.getGuessedSceneryDeviationCG();
            m_currentSceneryOffset = os;
            if (!os.isNull())
            {
                const CLength addValue = os * -1.0; // positive values means too high, negative values too low
                int changed = validSituations.addAltitudeOffset(addValue);
                Q_UNUSED(changed)
            }
        }
        else
        {
            m_currentSceneryOffset = CLength::null();
        }
        return validSituations;
    }

    template <typename Derived>
    bool CInterpolator<Derived>::presetGroundElevation(CAircraftSituation &situationToPreset, const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, const CAircraftSituationChange &change)
    {
        // IMPORTANT: we do not know what the situation will be (interpolated to), so we cannot transfer
        situationToPreset.resetGroundElevation();
        do
        {
            if (oldSituation.equalNormalVectorDouble(newSituation))
            {
                if (oldSituation.hasGroundElevation())
                {
                    // same positions, we can use existing elevation
                    // means we were not moving between old an new
                    situationToPreset.transferGroundElevationToMe(oldSituation, true);
                    break;
                }
            }

            const CLength distance = newSituation.calculateGreatCircleDistance(oldSituation);
            if (distance < newSituation.getDistancePerTime250ms(CElevationPlane::singlePointRadius()))
            {
                if (oldSituation.hasGroundElevation())
                {
                    // almost same positions, we can use existing elevation
                    situationToPreset.transferGroundElevationToMe(oldSituation, true);
                    break;
                }
            }

            if (change.hasElevationDevWithinAllowedRange())
            {
                // not much change in known elevations
                const CAltitudePair elvDevMean = change.getElevationStdDevAndMean();
                situationToPreset.setGroundElevation(elvDevMean.second, CAircraftSituation::SituationChange);
                break;
            }

            const CElevationPlane epInterpolated = CAircraftSituation::interpolatedElevation(CAircraftSituation::null(), oldSituation, newSituation, distance);
            if (!epInterpolated.isNull())
            {
                situationToPreset.setGroundElevation(epInterpolated, CAircraftSituation::Interpolated);
                break;
            }
        }
        while (false);
        return situationToPreset.hasGroundElevation();
    }

    template <typename Derived>
    void CInterpolator<Derived>::deferredInit()
    {
        if (m_model.hasModelString()) { return; } // set in-between
        this->initCorrespondingModel();
    }

    template <typename Derived>
    bool CInterpolator<Derived>::verifyInterpolationSituations(const CAircraftSituation &oldest, const CAircraftSituation &newer, const CAircraftSituation &latest, const CInterpolationAndRenderingSetupPerCallsign &setup)
    {
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return true; }
        CAircraftSituationList situations;

        // oldest last, null ignored
        if (!latest.isNull()) { situations.push_back(latest); }
        if (!newer.isNull()) { situations.push_back(newer); }
        if (!oldest.isNull()) { situations.push_back(oldest); }

        const bool sorted = situations.isSortedAdjustedLatestFirstWithoutNullPositions();
        if (setup.isNull() || !setup.isAircraftPartsEnabled()) { return sorted; }

        bool details = false;
        if (situations.containsOnGroundDetails(CAircraftSituation::InFromParts))
        {
            // if a client supports parts, all ground situations are supposed to be parts based
            details = situations.areAllOnGroundDetailsSame(CAircraftSituation::InFromParts);
            BLACK_VERIFY_X(details, Q_FUNC_INFO, "Once gnd.from parts -> always gnd. from parts");
        }

        for (const CAircraftSituation &s : situations)
        {
            if (!s.hasGroundElevation()) { continue; }
            BLACK_VERIFY_X(!s.getGroundElevation().isZeroEpsilonConsidered(), Q_FUNC_INFO, "Suspicous 0 gnd. value");
        }

        // check if middle situation is missing
        if (latest.hasGroundElevation() && oldest.hasGroundElevation())
        {
            BLACK_VERIFY_X(newer.hasGroundElevation(), Q_FUNC_INFO, "Middle ground elevation is missing");
        }

        // result
        return sorted && details;
    }

    template <typename Derived>
    const QStringList &CInterpolator<Derived>::getLogCategories()
    {
        static const QStringList cats { CLogCategories::interpolator() };
        return cats;
    }

    template <typename Derived>
    CInterpolationResult CInterpolator<Derived>::getInterpolation(qint64 currentTimeSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, uint32_t aircraftNumber)
    {
        CInterpolationResult result;

        const bool init = this->initIniterpolationStepData(currentTimeSinceEpoch, setup, aircraftNumber);
        Q_ASSERT_X(!m_currentInterpolationStatus.isInterpolated(), Q_FUNC_INFO, "Expect reset status");
        if (init || m_unitTest) // ignore failure in unittest
        {
            Q_ASSERT_X(m_currentTimeMsSinceEpoch > 0, Q_FUNC_INFO, "No valid timestamp, interpolator initialized?");
            const CAircraftSituation interpolatedSituation = this->getInterpolatedSituation();
            const CAircraftParts interpolatedParts = this->getInterpolatedOrGuessedParts(aircraftNumber);
            result.setValues(interpolatedSituation, interpolatedParts);
        }

        result.setStatus(m_currentInterpolationStatus, m_currentPartsStatus);
        return result;
    }

    template <typename Derived>
    CAircraftSituation CInterpolator<Derived>::getInterpolatedSituation()
    {
        Q_ASSERT_X(!m_currentInterpolationStatus.isInterpolated(), Q_FUNC_INFO, "Expect reset status");
        if (m_currentSituations.isEmpty())
        {
            m_lastSituation = CAircraftSituation::null();
            return CAircraftSituation::null();
        }

        // interpolant as function of derived class
        // CInterpolatorLinear::Interpolant or CInterpolatorSpline::Interpolant
        SituationLog log;
        const auto interpolant = derived()->getInterpolant(log);
        const bool isValidInterpolant = interpolant.isValid();

        CAircraftSituation currentSituation = m_lastSituation;
        CAircraftSituation::AltitudeCorrection altCorrection = CAircraftSituation::NoCorrection;

        bool isValidInterpolation = false;
        do
        {
            if (!isValidInterpolant) { break; }
            const CInterpolatorPbh pbh = interpolant.pbh();

            // init interpolated situation
            currentSituation = this->initInterpolatedSituation(pbh.getStartSituation(), pbh.getEndSituation());

            // Pitch bank heading first, so follow up steps could use those values
            currentSituation.setHeading(pbh.getHeading());
            currentSituation.setPitch(pbh.getPitch());
            currentSituation.setBank(pbh.getBank());
            currentSituation.setGroundSpeed(pbh.getGroundSpeed());

            // use derived interpolant function
            const bool interpolateGndFlag = pbh.getEndSituation().hasGroundDetailsForGndInterpolation() && pbh.getStartSituation().hasGroundDetailsForGndInterpolation();
            currentSituation = interpolant.interpolatePositionAndAltitude(currentSituation, interpolateGndFlag);
            if (currentSituation.isNull()) { break; }

            // if we get here and the vector is invalid it means we haven't handled it correctly in one of the interpolators
            if (!currentSituation.isValidVectorRange())
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild()) { BLACK_VERIFY_X(false, Q_FUNC_INFO, "Invalid interpolation situation"); }
                return CAircraftSituation::null();
            }

            // GND flag.
            if (!interpolateGndFlag) { CAircraftSituationChange::null().guessOnGround(currentSituation, m_model); }

            // as we now have the position and can interpolate elevation
            currentSituation.interpolateElevation(pbh.getStartSituation(), pbh.getEndSituation());
            if (!currentSituation.hasGroundElevation())
            {
                // we still have no elevation
                const CLength radius = currentSituation.getDistancePerTime250ms(CElevationPlane::singlePointRadius());
                if (!m_lastSituation.transferGroundElevationFromMe(currentSituation, radius))
                {
                    if (currentSituation.canLikelySkipNearGroundInterpolation())
                    {
                        // skipped
                    }
                    else
                    {
                        const CElevationPlane groundElevation = this->findClosestElevationWithinRange(currentSituation, radius);
                        m_lastSituation.setGroundElevationChecked(groundElevation, CAircraftSituation::FromCache);
                    }
                }
            }

            // correct altitude itself
            if (!interpolateGndFlag && currentSituation.getOnGroundDetails() != CAircraftSituation::OnGroundByGuessing)
            {
                // just in case
                altCorrection = currentSituation.correctAltitude(true); // we have CG set
            }

            // correct pitch on ground
            if (currentSituation.isOnGround())
            {
                const CAngle correctedPitchOnGround = m_currentSetup.getPitchOnGround();
                if (!correctedPitchOnGround.isNull())
                {
                    currentSituation.setPitch(correctedPitchOnGround);
                }
            }

            isValidInterpolation = true;
        }
        while (false);

        const bool valid = isValidInterpolant && isValidInterpolation;
        if (!valid)
        {
            // further handling could go here, mainly we continue with last situation
            m_invalidSituations++;

            // avoid flooding of log.
            if (m_currentTimeMsSinceEpoch - m_lastInvalidLogTs > m_lastSituation.getTimeOffsetMs())
            {
                m_lastInvalidLogTs = m_currentTimeMsSinceEpoch;
                const bool noSituation = m_lastSituation.isNull();

                // Problem 1, we have no "last situation"
                // Problem 2, "it takes too long to recover"
                CStatusMessage m;
                if (noSituation)
                {
                    m = CStatusMessage(this).warning(u"No situation #%1 for interpolation reported for '%2' (Interpolant: %3 interpolation: %4)") << m_invalidSituations << m_callsign.asString() << boolToTrueFalse(isValidInterpolant) << boolToTrueFalse(isValidInterpolation);
                }
                else
                {
                    const qint64 diff = m_currentTimeMsSinceEpoch - currentSituation.getAdjustedMSecsSinceEpoch();
                    m = CStatusMessage(this).warning(u"Invalid situation, diff. %1ms #%2 for interpolation reported for '%3' (Interpolant: %4 interpolation: %5)") << diff << m_invalidSituations << m_callsign.asString() << boolToTrueFalse(isValidInterpolant) << boolToTrueFalse(isValidInterpolation);
                }
                if (!m.isEmpty())
                {
                    if (m_interpolationMessages.sizeInt() == 2)
                    {
                        // display second message as a hint in the general log
                        // we DO NOT display the first message, as this can happen due to pilot logging off
                        // if it happens twice we consider it worth displaying
                        CLogMessage::preformatted(m);
                    }
                    m_interpolationMessages.push_back(m);
                }
            }
        } // valid?

        // situation and status
        if (valid)
        {
            Q_ASSERT_X(currentSituation.hasMSLGeodeticHeight(), Q_FUNC_INFO, "No MSL altitude");
            m_lastSituation = currentSituation;
            m_currentInterpolationStatus.setInterpolatedAndCheckSituation(valid, currentSituation);
        }
        else
        {
            currentSituation = m_lastSituation;
            m_currentInterpolationStatus.setSameSituation(true);
            m_currentInterpolationStatus.setInterpolatedAndCheckSituation(valid, currentSituation);
        }

        // logging
        if (this->doLogging())
        {
            log.tsCurrent = m_currentTimeMsSinceEpoch;
            log.callsign = m_callsign;
            log.groundFactor = currentSituation.getOnGroundFactor();
            log.altCorrection = CAircraftSituation::altitudeCorrectionToString(altCorrection);
            log.situationCurrent = currentSituation;
            log.interpolantRecalc = interpolant.isRecalculated();
            log.change = m_pastSituationsChange;
            log.usedSetup = m_currentSetup;
            log.elevationInfo = this->getElevationsFoundMissedInfo();
            log.cgAboveGround = currentSituation.getCG();
            log.sceneryOffset = m_currentSceneryOffset;
            log.noInvalidSituations = m_invalidSituations;
            log.noNetworkSituations = m_currentSituations.sizeInt();
            log.useParts = this->isRemoteAircraftSupportingParts(m_callsign);
            m_logger->logInterpolation(log);

            // if (log.interpolantRecalc) { CLogMessage(this).debug(u"Recalc %1") << log.callsign.asString(); }
        }

        // bye
        return currentSituation;
    }

    template <typename Derived>
    CAircraftParts CInterpolator<Derived>::getInterpolatedParts()
    {
        // (!) this code is used by linear and spline interpolator

        // Parts are supposed to be in correct order, latest first
        const CAircraftPartsList validParts = this->remoteAircraftParts(m_callsign);

        // log for empty parts aircraft parts
        if (validParts.isEmpty())
        {
            static const CAircraftParts emptyParts;
            this->logParts(emptyParts, validParts.size(), true);
            return emptyParts;
        }

        m_currentPartsStatus.setSupportsParts(true);
        CAircraftParts currentParts;
        do
        {
            // find the first parts earlier than the current time
            const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [=](auto &&p) { return p.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch; });
            const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
            const auto partsOlder = makeRange(pivot, validParts.end());

            // if (partsOlder.isEmpty()) { currentParts = *(partsNewer.end() - 1); break; }
            if (partsOlder.isEmpty())
            {
                currentParts = *(partsNewer.begin());
                break;
            }
            currentParts = partsOlder.front(); // latest older parts
        }
        while (false);

        this->logParts(currentParts, validParts.size(), false);
        return currentParts;
    }

    template <typename Derived>
    CAircraftParts CInterpolator<Derived>::getInterpolatedOrGuessedParts(int aircraftNumber)
    {
        Q_ASSERT_X(m_partsToSituationInterpolationRatio >= 1 && m_partsToSituationInterpolationRatio < 11, Q_FUNC_INFO, "Wrong ratio");
        const bool needParts = m_unitTest || m_lastParts.isNull();
        const bool doInterpolation = needParts || ((m_interpolatedSituationsCounter + aircraftNumber) % m_partsToSituationInterpolationRatio == 0);
        const bool doGuess = needParts || ((m_interpolatedSituationsCounter + aircraftNumber) % m_partsToSituationGuessingRatio == 0);

        if (!doGuess && !doInterpolation)
        {
            // reuse
            return this->logAndReturnNullParts("neither guess nor interpolation", true);
        }

        CAircraftParts parts = CAircraftParts::null();
        if (m_currentSetup.isAircraftPartsEnabled())
        {
            // this already logs and sets status
            parts = this->getInterpolatedParts();
        }

        // if we have supported parts, we skip this step, but it can happen the parts are still empty
        if (!m_currentPartsStatus.isSupportingParts())
        {
            if (!doGuess)
            {
                return this->logAndReturnNullParts("not supporting parts, and marked for guessing", true);
            }

            // check if model has been thru model matching
            if (!m_lastSituation.isNull())
            {
                parts = guessParts(m_lastSituation, m_pastSituationsChange, m_model);
                this->logParts(parts, 0, false);
            }
            else
            {
                // quite normal initial situation, just return NULL
                return this->logAndReturnNullParts("guessing, but no situation yet", false);
            }
        }

        m_lastParts = parts;
        m_lastPartsStatus = m_currentPartsStatus;
        return parts;
    }

    template <typename Derived>
    const CAircraftParts &CInterpolator<Derived>::logAndReturnNullParts(const QString &info, bool log)
    {
        if (!m_lastParts.isNull())
        {
            m_currentPartsStatus = m_lastPartsStatus;
            m_currentPartsStatus.setReusedParts(true);
            return m_lastParts;
        }

        if (log)
        {
            const CStatusMessage m = CStatusMessage(this).warning(u"NULL parts reported for '%1', '%2')") << m_callsign.asString() << info;
            if (m_interpolationMessages.isEmpty()) { CLogMessage::preformatted(m); }
            m_interpolationMessages.push_back(m);
        }
        m_currentPartsStatus = {};
        return CAircraftParts::null();
    }

    template <typename Derived>
    bool CInterpolator<Derived>::doLogging() const
    {
        return this->hasAttachedLogger() && m_currentSetup.logInterpolation();
    }

    template <typename Derived>
    CAircraftParts CInterpolator<Derived>::guessParts(const CAircraftSituation &situation, const CAircraftSituationChange &change, const CAircraftModel &model)
    {
        CAircraftParts parts;
        parts.setMSecsSinceEpoch(situation.getMSecsSinceEpoch());
        parts.setTimeOffsetMs(situation.getTimeOffsetMs());
        parts.setPartsDetails(CAircraftParts::GuessedParts);
        parts.setLights(situation.guessLights());

        QString *details = /*CBuildConfig::isLocalDeveloperDebugBuild() ? &parts.m_guessingDetails :*/ nullptr;

        CAircraftEngineList engines;
        const bool vtol = model.isVtol();
        const int engineCount = model.getEngineCount();
        CSpeed guessedVRotate = CSpeed::null();
        CLength guessedCG = model.getCG();
        model.getAircraftIcaoCode().guessModelParameters(guessedCG, guessedVRotate);

        if (situation.getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails)
        {
            do
            {
                // set some reasonable values
                const bool isOnGround = situation.isOnGround();
                engines.initEngines(engineCount, !isOnGround || situation.isMoving());
                parts.setGearDown(isOnGround);
                parts.setSpoilersOut(false);
                parts.setEngines(engines);

                if (!change.isNull())
                {
                    if (change.isConstDecelarating())
                    {
                        parts.setSpoilersOut(true);
                        parts.setFlapsPercent(10);
                        break;
                    }
                }

                const CSpeed slowSpeed = guessedVRotate * 0.30;
                if (situation.getGroundSpeed() < slowSpeed)
                {
                    if (details) { *details += u"slow speed <" % slowSpeed.valueRoundedWithUnit(1) % u" on ground"; }
                    parts.setFlapsPercent(0);
                    break;
                }
                else
                {
                    if (details) { *details += u"faster speed >" % slowSpeed.valueRoundedWithUnit(1) % u" on ground"; }
                    parts.setFlapsPercent(0);
                    break;
                }
            }
            while (false);
        }
        else
        {
            if (details) { *details = QStringLiteral("no ground info"); }

            // no idea if on ground or not
            engines.initEngines(engineCount, true);
            parts.setEngines(engines);
            parts.setGearDown(true);
            parts.setSpoilersOut(false);
        }

        const double pitchDeg = situation.getPitch().value(CAngleUnit::deg());
        const bool isLikelyTakeOffOrClimbing = change.isNull() ? pitchDeg > 20 : (change.isRotatingUp() || change.isConstAscending());
        const bool isLikelyLanding = change.isNull() ? false : change.isConstDescending();

        if (situation.hasGroundElevation())
        {
            const CLength aboveGnd = situation.getHeightAboveGround();
            if (aboveGnd.isNull() || std::isnan(aboveGnd.value()))
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "above gnd.is null");
                return parts;
            }

            const double nearGround1Ft = 300;
            const double nearGround2Ft = isLikelyTakeOffOrClimbing ? 500 : 1000;
            const double aGroundFt = aboveGnd.value(CLengthUnit::ft());
            static const QString detailsInfo("above ground: %1ft near grounds: %2ft %3ft likely takeoff: %4 likely landing: %5");

            if (details) { *details = detailsInfo.arg(aGroundFt).arg(nearGround1Ft).arg(nearGround2Ft).arg(boolToYesNo(isLikelyTakeOffOrClimbing), boolToYesNo(isLikelyLanding)); }
            if (aGroundFt < nearGround1Ft)
            {
                if (details) { details->prepend(QStringLiteral("near ground: ")); }
                parts.setGearDown(true);
                parts.setFlapsPercent(25);
            }
            else if (aGroundFt < nearGround2Ft)
            {
                if (details) { details->prepend(QStringLiteral("2nd layer: ")); }
                const bool gearDown = !isLikelyTakeOffOrClimbing && (situation.getGroundSpeed() < guessedVRotate || isLikelyLanding);
                parts.setGearDown(gearDown);
                parts.setFlapsPercent(10);
            }
            else
            {
                if (details) { details->prepend(QStringLiteral("airborne: ")); }
                parts.setGearDown(false);
                parts.setFlapsPercent(0);
            }
        }
        else
        {
            if (situation.getOnGroundDetails() != CAircraftSituation::NotSetGroundDetails)
            {
                // we have no ground elevation but a ground info
                if (situation.getOnGroundDetails() == CAircraftSituation::OnGroundByGuessing)
                {
                    // should be OK
                    if (details) { *details = QStringLiteral("on ground, no elv."); }
                }
                else
                {
                    if (!vtol)
                    {
                        const bool gearDown = situation.getGroundSpeed() < guessedVRotate;
                        parts.setGearDown(gearDown);
                        if (details) { *details = QStringLiteral("not on ground elv., gs < ") + guessedVRotate.valueRoundedWithUnit(1); }
                    }
                }
            }
        }

        return parts;
    }

    template <typename Derived>
    void CInterpolator<Derived>::logParts(const CAircraftParts &parts, int partsNo, bool empty) const
    {
        if (!this->doLogging()) { return; }
        PartsLog logInfo;
        logInfo.callsign = m_callsign;
        logInfo.noNetworkParts = partsNo;
        logInfo.tsCurrent = m_currentTimeMsSinceEpoch;
        logInfo.parts = parts;
        logInfo.empty = empty;
        m_logger->logParts(logInfo);
    }

    template <typename Derived>
    QString CInterpolator<Derived>::getInterpolatorInfo() const
    {
        return QStringLiteral("Callsign: ") %
               m_callsign.asString() %
               QStringLiteral(" situations: ") %
               QString::number(this->remoteAircraftSituationsCount(m_callsign)) %
               QStringLiteral(" parts: ") %
               QString::number(this->remoteAircraftPartsCount(m_callsign)) %
               QStringLiteral(" 1st interpolation: ") %
               boolToYesNo(m_lastSituation.isNull());
    }

    template <typename Derived>
    void CInterpolator<Derived>::resetLastInterpolation()
    {
        m_lastSituation.setNull();
    }

    template <typename Derived>
    bool CInterpolator<Derived>::initIniterpolationStepData(qint64 currentTimeSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber)
    {
        Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

        const qint64 lastModifed = this->situationsLastModified(m_callsign);
        const bool slowUpdateStep = (((m_interpolatedSituationsCounter + aircraftNumber) % 25) == 0); // flag when parts are updated, which need not to be updated every time
        const bool changedSituations = lastModifed > m_situationsLastModified;

        m_currentTimeMsSinceEpoch = currentTimeSinceEpoch;
        m_currentInterpolationStatus = {};
        m_currentPartsStatus = {};
        m_currentSetup = setup;

        if (changedSituations)
        {
            m_situationsLastModified = lastModifed;
            m_currentSituations = this->remoteAircraftSituationsAndChange(setup); // only update when needed
        }

        if (!m_model.hasCG() || slowUpdateStep)
        {
            this->getAndFetchModelCG(CLength::null()); // update CG
        }

        bool success = false;
        const int situationsSize = m_currentSituations.sizeInt();
        m_currentInterpolationStatus.setSituationsCount(situationsSize);
        if (m_currentSituations.isEmpty())
        {
            const bool inRange = this->isAircraftInRange(m_callsign);
            m_lastSituation = CAircraftSituation::null(); // no interpolation possible for that step
            static const QString extraNoSituations("No situations, but remote aircraft '%1'");
            static const QString extraNoRemoteAircraft("Unknown remote aircraft: '%1'");
            m_currentInterpolationStatus.setExtraInfo((inRange ? extraNoSituations : extraNoRemoteAircraft).arg(m_callsign.asString()));
        }
        else
        {
            success = true;
            m_interpolatedSituationsCounter++; // counter updated in initIniterpolationStepData

            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            if (!CBuildConfig::isReleaseBuild())
            {
                Q_ASSERT_X(m_currentSituations.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Wrong sort order");
                Q_ASSERT_X(m_currentSituations.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
            }
        }

        return success;
    }

    template <typename Derived>
    CAircraftSituation CInterpolator<Derived>::initInterpolatedSituation(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation) const
    {
        if (m_currentSituations.isEmpty()) { return CAircraftSituation::null(); }

        CAircraftSituation currentSituation = m_lastSituation.isNull() ? m_currentSituations.front() : m_lastSituation;
        if (currentSituation.getCallsign() != m_callsign)
        {
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong callsign");
            currentSituation.setCallsign(m_callsign);
        }

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(currentSituation.isValidVectorRange(), Q_FUNC_INFO, "Invalid range");
        }

        // preset elevation here, as we do not know where the situation will be after the interpolation step!
        const bool preset = presetGroundElevation(currentSituation, oldSituation, newSituation, m_pastSituationsChange);
        Q_UNUSED(preset)

        // fetch CG once
        const CLength cg(this->getModelCG());
        currentSituation.setCG(cg);
        return currentSituation;
    }

    template <typename Derived>
    void CInterpolator<Derived>::initCorrespondingModel(const CAircraftModel &model)
    {
        if (model.hasModelString())
        {
            m_model = model;
        }
        else
        {
            CAircraftModel foundModel = this->getAircraftInRangeForCallsign(m_callsign).getModel();
            if (foundModel.hasModelString())
            {
                m_model = foundModel;
            }
        }
        this->getAndFetchModelCG(model.getCG());
    }

    template <typename Derived>
    void CInterpolator<Derived>::markAsUnitTest()
    {
        m_unitTest = true;
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    //! \cond PRIVATE
    template class CInterpolator<CInterpolatorLinear>;
    template class CInterpolator<CInterpolatorSpline>;
    //! \endcond
} // namespace
