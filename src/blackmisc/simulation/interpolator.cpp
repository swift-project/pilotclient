/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolator.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolatorlinear.h"
#include "blackmisc/simulation/interpolatorspline.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include <QTimer>
#include <QDateTime>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        template <typename Derived>
        CInterpolator<Derived>::CInterpolator(const CCallsign &callsign,
                                              ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider,
                                              IRemoteAircraftProvider *remoteProvider,
                                              CInterpolationLogger *logger) : m_callsign(callsign)
        {
            // normally when created m_cg is still null since there is no CG in the provider yet

            if (simEnvProvider) { this->setSimulationEnvironmentProvider(simEnvProvider); }
            if (setupProvider)  { this->setInterpolationSetupProvider(setupProvider); }
            if (remoteProvider)
            {
                this->setRemoteAircraftProvider(remoteProvider);
                QObject::connect(&m_initTimer, &QTimer::timeout, [ = ] { this->deferredInit(); });
                m_initTimer.setSingleShot(true);
                m_initTimer.start(2500);
            }
            this->attachLogger(logger);
        }

        template<typename Derived>
        CLength CInterpolator<Derived>::getAndFetchModelCG()
        {
            const CLength cg = this->getCG(m_callsign);
            m_model.setCG(cg);
            m_model.setCallsign(m_callsign);
            return cg;
        }

        template<typename Derived>
        double CInterpolator<Derived>::groundInterpolationFactor()
        {
            // done here so we can change value without "larfer" recompilations
            static constexpr double f = 0.95;
            return f;
        }

        template<typename Derived>
        CAircraftSituationList CInterpolator<Derived>::remoteAircraftSituationsAndChange(const CInterpolationAndRenderingSetupPerCallsign &setup)
        {
            // const bool vtol = setup.isForcingVtolInterpolation() || m_model.isVtol();
            CAircraftSituationList validSituations = this->remoteAircraftSituations(m_callsign);

            // get the changes, we need the second value as we want to look in the past
            // the first value is already based on the latest situation
            const CAircraftSituationChangeList changes = this->remoteAircraftSituationChanges(m_callsign);
            m_pastSituationsChange = (changes.size() > 1) ? changes[1] : CAircraftSituationChange::null();

            if (setup.isFixingSceneryOffset() && m_pastSituationsChange.hasSceneryDeviation() && m_model.hasCG())
            {
                const CLength os = m_pastSituationsChange.getGuessedSceneryDeviationCG();
                m_currentSceneryOffset = os;
                if (!os.isNull())
                {
                    const CLength addValue = os * -1.0; // positive values means too high, negative values too low
                    int changed = validSituations.addAltitudeOffset(addValue);
                    Q_UNUSED(changed);
                }
            }
            else
            {
                m_currentSceneryOffset = CLength::null();
            }
            return validSituations;
        }

        template<typename Derived>
        void CInterpolator<Derived>::deferredInit()
        {
            if (m_model.hasModelString()) { return; } // set in-between
            this->initCorrespondingModel();
        }

        template<typename Derived>
        bool CInterpolator<Derived>::verifyInterpolationSituations(const CAircraftSituation &oldest, const CAircraftSituation &newer, const CAircraftSituation &latest, const CInterpolationAndRenderingSetupPerCallsign &setup)
        {
            if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return true; }
            CAircraftSituationList situations;

            // oldest last, null ignored
            if (!latest.isNull()) { situations.push_back(latest); }
            if (!newer.isNull())  { situations.push_back(newer); }
            if (!oldest.isNull()) { situations.push_back(oldest); }

            const bool sorted = situations.isSortedAdjustedLatestFirstWithoutNullPositions();
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(sorted, Q_FUNC_INFO, "Wrong adjusted timestamp order");
            }

            if (setup.isNull() || !setup.isAircraftPartsEnabled()) { return sorted; }

            bool details = false;
            if (situations.containsOnGroundDetails(CAircraftSituation::InFromParts))
            {
                // if a client supports parts, all situations are supposed to be parts based
                details = situations.areAllOnGroundDetailsSame(CAircraftSituation::InFromParts);
                BLACK_VERIFY_X(details, Q_FUNC_INFO, "Once gnd.from parts -> always gnd. from parts");
            }

            // result
            return sorted && details;
        }

        template <typename Derived>
        const CLogCategoryList &CInterpolator<Derived>::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::interpolator() };
            return cats;
        }

        template<typename Derived>
        CInterpolationResult CInterpolator<Derived>::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber)
        {
            CInterpolationResult result;
            do
            {
                // make sure we can also interpolate parts only (needed in unit tests)
                if (aircraftNumber < 0) { aircraftNumber = 0; }
                const bool init = this->initIniterpolationStepData(currentTimeSinceEpoc, setup, aircraftNumber);
                if (!m_unitTest && !init) { break; } // failure in real scenarios, unit tests move on

                Q_ASSERT_X(m_currentTimeMsSinceEpoch > 0, Q_FUNC_INFO, "No valid timestamp, interpolator initialized?");
                const CAircraftSituation interpolatedSituation = this->getInterpolatedSituation();
                const CAircraftParts interpolatedParts = this->getInterpolatedOrGuessedParts(aircraftNumber);

                result.setValues(interpolatedSituation, interpolatedParts);
            }
            while (false);

            result.setStatus(m_currentInterpolationStatus, m_currentPartsStatus);
            return result;
        }

        template <typename Derived>
        CAircraftSituation CInterpolator<Derived>::getInterpolatedSituation()
        {
            if (m_currentSituations.isEmpty())
            {
                m_lastSituation = CAircraftSituation::null();
                return CAircraftSituation::null();
            }

            // interpolant as function of derived class
            // CInterpolatorLinear::Interpolant or CInterpolatorSpline::Interpolant
            SituationLog log;
            const auto interpolant = derived()->getInterpolant(log);
            const CInterpolatorPbh pbh = interpolant.pbh();

            // init interpolated situation
            CAircraftSituation currentSituation = this->initInterpolatedSituation(pbh.getOldSituation(), pbh.getNewSituation());

            // Pitch bank heading first, so follow up steps could use those values
            currentSituation.setHeading(pbh.getHeading());
            currentSituation.setPitch(pbh.getPitch());
            currentSituation.setBank(pbh.getBank());
            currentSituation.setGroundSpeed(pbh.getGroundSpeed());

            // use derived interpolant function
            const bool interpolateGndFlag = pbh.getNewSituation().hasGroundDetailsForGndInterpolation() && pbh.getOldSituation().hasGroundDetailsForGndInterpolation();
            currentSituation = interpolant.interpolatePositionAndAltitude(currentSituation, interpolateGndFlag);
            if (!interpolateGndFlag) { currentSituation.guessOnGround(CAircraftSituationChange::null(), m_model); }

            // as we now have the position and can interpolate elevation
            currentSituation.interpolateElevation(pbh.getOldSituation(), pbh.getNewSituation());
            if (!currentSituation.hasGroundElevation())
            {
                // we still have no elevation
                const CLength radius = currentSituation.getDistancePerTime250ms();
                if (!m_lastSituation.transferGroundElevation(currentSituation, radius))
                {
                    const CElevationPlane groundElevation = this->findClosestElevationWithinRange(currentSituation, radius);
                    m_lastSituation.setGroundElevationChecked(groundElevation, CAircraftSituation::FromCache);
                }
            }

            // correct altitude itself
            CAircraftSituation::AltitudeCorrection altCorrection = CAircraftSituation::NoCorrection;
            if (!interpolateGndFlag && currentSituation.getOnGroundDetails() != CAircraftSituation::OnGroundByGuessing)
            {
                // just in case
                altCorrection = currentSituation.correctAltitude(true); // we have CG set
            }

            // status
            m_currentInterpolationStatus.setInterpolatedAndCheckSituation(true, currentSituation);

            // logging
            if (this->doLogging())
            {
                static const QString elv("found %1 missed %2");
                const QPair<int, int> elvStats = this->getElevationsFoundMissed();
                log.tsCurrent = m_currentTimeMsSinceEpoch;
                log.callsign = m_callsign;
                log.groundFactor = currentSituation.getOnGroundFactor();
                log.altCorrection = CAircraftSituation::altitudeCorrectionToString(altCorrection);
                log.situationCurrent = currentSituation;
                log.change = m_pastSituationsChange;
                log.usedSetup = m_currentSetup;
                log.elevationInfo = elv.arg(elvStats.first).arg(elvStats.second);
                log.cgAboveGround = currentSituation.getCG();
                log.sceneryOffset = m_currentSceneryOffset;
                m_logger->logInterpolation(log);
            }

            // bye
            m_lastSituation = currentSituation;
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
                const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto &&p) { return p.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch; });
                const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
                const auto partsOlder = makeRange(pivot, validParts.end());

                // if (partsOlder.isEmpty()) { currentParts = *(partsNewer.end() - 1); break; }
                if (partsOlder.isEmpty()) { currentParts = *(partsNewer.begin()); break; }
                currentParts = partsOlder.front(); // latest older parts
            }
            while (false);

            this->logParts(currentParts, validParts.size(), false);
            return currentParts;
        }

        template<typename Derived>
        CAircraftParts CInterpolator<Derived>::getInterpolatedOrGuessedParts(int aircraftNumber)
        {
            Q_ASSERT_X(m_partsToSituationInterpolationRatio >= 1 && m_partsToSituationInterpolationRatio < 11, Q_FUNC_INFO, "Wrong ratio");
            const bool needParts = m_unitTest || m_lastParts.isNull();
            const bool doInterpolation = needParts || ((m_interpolatedSituationsCounter + aircraftNumber) % m_partsToSituationInterpolationRatio == 0);
            const bool doGuess = needParts || ((m_interpolatedSituationsCounter + aircraftNumber) % m_partsToSituationGuessingRatio == 0);

            if (!doGuess && !doInterpolation)
            {
                m_currentPartsStatus = m_lastPartsStatus;
                m_currentPartsStatus.setReusedParts(true);
                return m_lastParts;
            }

            CAircraftParts parts;
            if (m_currentSetup.isAircraftPartsEnabled())
            {
                // this already logs
                parts = this->getInterpolatedParts();
            }

            // if we have supported parts, we skip this step, but it can happen
            // the parts are still empty
            if (!m_currentPartsStatus.isSupportingParts())
            {
                if (!doGuess)
                {
                    m_currentPartsStatus = m_lastPartsStatus;
                    m_currentPartsStatus.setReusedParts(true);
                    return m_lastParts;
                }

                // check if model has been thru model matching
                parts.guessParts(m_lastSituation, m_pastSituationsChange, m_model);
                this->logParts(parts, 0, false);
            }

            m_lastParts = parts;
            m_lastPartsStatus = m_currentPartsStatus;
            return parts;
        }

        template<typename Derived>
        bool CInterpolator<Derived>::doLogging() const
        {
            return this->hasAttachedLogger() &&  m_currentSetup.logInterpolation();
        }

        template<typename Derived>
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

        template<typename Derived>
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

        template<typename Derived>
        void CInterpolator<Derived>::resetLastInterpolation()
        {
            m_lastSituation.setNull();
        }

        template<typename Derived>
        void CInterpolator<Derived>::clear()
        {
            this->resetLastInterpolation();
            m_model = CAircraftModel();
            m_currentSceneryOffset = CLength::null();
            m_pastSituationsChange = CAircraftSituationChange::null();
            m_currentSituations.clear();
            m_currentTimeMsSinceEpoch = -1;
            m_situationsLastModified = -1;
            m_situationsLastModifiedUsed = -1;
            m_currentInterpolationStatus.reset();
            m_currentPartsStatus.reset();
            m_interpolatedSituationsCounter = 0;
        }

        template<typename Derived>
        bool CInterpolator<Derived>::initIniterpolationStepData(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber)
        {
            Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

            const qint64 lastModifed = this->situationsLastModified(m_callsign);
            const bool slowUpdateStep = (((m_interpolatedSituationsCounter + aircraftNumber) % 25) == 0); // flag when parts are updated, which need not to be updated every time
            const bool changedSetup = m_currentSetup != setup;
            const bool changedSituations = lastModifed > m_situationsLastModified;

            m_currentTimeMsSinceEpoch = currentTimeSinceEpoc;
            m_currentInterpolationStatus.reset();
            m_currentPartsStatus.reset();

            if (changedSetup || changedSituations)
            {
                m_currentSetup = setup;
                m_situationsLastModified = lastModifed;
                m_currentSituations = this->remoteAircraftSituationsAndChange(setup); // only update when needed
            }

            if (!m_model.hasCG() || slowUpdateStep)
            {
                this->getAndFetchModelCG(); // update CG
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

        template<typename Derived>
        CAircraftSituation CInterpolator<Derived>::initInterpolatedSituation(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation) const
        {
            if (m_currentSituations.isEmpty()) { return CAircraftSituation::null(); }
            CAircraftSituation currentSituation = m_lastSituation.isNull() ? m_currentSituations.front() : m_lastSituation;
            if (currentSituation.getCallsign() != m_callsign)
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong callsign");
                currentSituation.setCallsign(m_callsign);
            }

            // preset elevation here, as we do not know where the situation will be after the interpolation step!
            const bool preset = currentSituation.presetGroundElevation(oldSituation, newSituation, m_pastSituationsChange);
            Q_UNUSED(preset);

            // fetch CG once
            const CLength cg(this->getModelCG());
            currentSituation.setCG(cg);
            return currentSituation;
        }

        template<typename Derived>
        void CInterpolator<Derived>::initCorrespondingModel(const CAircraftModel &model)
        {
            if (model.hasModelString())
            {
                m_model = model;
            }
            else
            {
                CAircraftModel model = this->getAircraftInRangeForCallsign(m_callsign).getModel();
                if (model.hasModelString())
                {
                    m_model = model;
                }
            }
            this->getAndFetchModelCG();
        }

        CInterpolationResult::CInterpolationResult()
        {
            this->reset();
        }

        void CInterpolationResult::setValues(const CAircraftSituation &situation, const CAircraftParts &parts)
        {
            m_interpolatedSituation = situation;
            m_interpolatedParts = parts;
        }

        void CInterpolationResult::setStatus(const CInterpolationStatus &interpolation, const CPartsStatus &parts)
        {
            m_interpolationStatus = interpolation;
            m_partsStatus = parts;
        }

        void CInterpolationResult::reset()
        {
            m_interpolatedSituation = CAircraftSituation::null();
            m_interpolatedParts = CAircraftParts::null();
            m_interpolationStatus.reset();
            m_partsStatus.reset();
        }

        void CInterpolationStatus::setExtraInfo(const QString &info)
        {
            m_extraInfo = info;
        }

        void CInterpolationStatus::setInterpolatedAndCheckSituation(bool succeeded, const CAircraftSituation &situation)
        {
            m_isInterpolated = succeeded;
            this->checkIfValidSituation(situation);
        }

        void CInterpolationStatus::checkIfValidSituation(const CAircraftSituation &situation)
        {
            m_isValidSituation = !situation.isPositionOrAltitudeNull();
        }

        bool CInterpolationStatus::hasValidInterpolatedSituation() const
        {
            return m_isInterpolated && m_isValidSituation;
        }

        void CInterpolationStatus::reset()
        {
            m_extraInfo.clear();
            m_isValidSituation = false;
            m_isInterpolated = false;
            m_isSameSituation = false;
            m_situations = -1;
        }

        QString CInterpolationStatus::toQString() const
        {
            return QStringLiteral("Interpolated: ") % boolToYesNo(m_isInterpolated) %
                   QStringLiteral(" | situations: ") % QString::number(m_situations) %
                   QStringLiteral(" | situation valid: ") % boolToYesNo(m_isValidSituation) %
                   QStringLiteral(" | same: ") % boolToYesNo(m_isSameSituation) %
                   (
                       m_extraInfo.isEmpty() ? QStringLiteral("") : QStringLiteral(" info: ") % m_extraInfo
                   );
        }

        void CPartsStatus::reset()
        {
            m_supportsParts = false;
            m_resusedParts = false;
            m_isSameParts = false;
        }

        QString CPartsStatus::toQString() const
        {
            return QStringLiteral("Supported parts: ") % boolToYesNo(m_supportsParts) %
                   QStringLiteral(" | reused: ") % boolToYesNo(m_resusedParts) %
                   QStringLiteral(" | same: ") % boolToYesNo(m_isSameParts);
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CInterpolator<CInterpolatorLinear>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CInterpolator<CInterpolatorSpline>;
        //! \endcond
    } // namespace
} // namespace
