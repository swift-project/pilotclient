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
        CAircraftSituationList CInterpolator<Derived>::remoteAircraftSituationsAndChange(bool useSceneryOffset)
        {
            CAircraftSituationList validSituations = this->remoteAircraftSituations(m_callsign);
            m_currentSituationChange = CAircraftSituationChange(validSituations, true, true);
            if (useSceneryOffset && m_currentSituationChange.hasSceneryDeviation() && m_model.hasCG())
            {
                const CLength os = m_currentSituationChange.getGuessedSceneryDeviation(m_model.getCG());
                validSituations.addAltitudeOffset(os);
                m_currentSituationChange = CAircraftSituationChange(validSituations, true, true); // recalculate
                m_currentSceneryOffset = os;
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
        CInterpolationResult CInterpolator<Derived>::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup)
        {
            CInterpolationResult result;
            do
            {
                if (!this->initIniterpolationStepData(currentTimeSinceEpoc, setup)) { break; }

                // make sure we can also interpolate parts only (needed in unit tests for instance)
                const CAircraftSituation interpolatedSituation = this->getInterpolatedSituation();
                const CAircraftParts interpolatedParts = this->getInterpolatedOrGuessedParts();

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
                m_lastInterpolation = CAircraftSituation::null();
                return CAircraftSituation::null();
            }

            const CAircraftSituation latest = m_currentSituations.front();
            CAircraftSituation currentSituation = m_lastInterpolation.isNull() ? latest : m_lastInterpolation;
            if (currentSituation.getCallsign() != m_callsign)
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong callsign");
                currentSituation.setCallsign(m_callsign);
            }

            // set elevation if available
            if (!currentSituation.hasGroundElevation())
            {
                const CElevationPlane currentGroundElevation = this->findClosestElevationWithinRange(currentSituation, currentSituation.getDistancePerTime(1000));
                currentSituation.setGroundElevationChecked(currentGroundElevation); // set as default
            }

            // fetch CG once
            const CLength cg(this->getModelCG());
            currentSituation.setCG(cg);

            // interpolant function from derived class
            // CInterpolatorLinear::Interpolant or CInterpolatorSpline::Interpolant
            SituationLog log;
            const auto interpolant = derived()->getInterpolant(log);

            // succeeded so far?
            if (!m_currentInterpolationStatus.isInterpolated())
            {
                m_currentInterpolationStatus.checkIfValidSituation(currentSituation);
                return currentSituation;
            }

            // Pitch bank heading
            // first, so follow up steps could use those values
            const CInterpolatorPbh pbh = interpolant.pbh();
            currentSituation.setHeading(pbh.getHeading());
            currentSituation.setPitch(pbh.getPitch());
            currentSituation.setBank(pbh.getBank());
            currentSituation.setGroundSpeed(pbh.getGroundSpeed());

            // use derived interpolant function
            const bool interpolateGndFlag = pbh.getNewSituation().hasGroundDetailsForGndInterpolation() && pbh.getOldSituation().hasGroundDetailsForGndInterpolation();
            currentSituation = interpolant.interpolatePositionAndAltitude(currentSituation, interpolateGndFlag);
            if (!interpolateGndFlag) { currentSituation.guessOnGround(CAircraftSituationChange::null(), m_model); }

            // correct itself
            CAircraftSituation::AltitudeCorrection altCorrection = CAircraftSituation::NoCorrection;
            if (!interpolateGndFlag && currentSituation.getOnGroundDetails() != CAircraftSituation::OnGroundByGuessing)
            {
                // just in case
                altCorrection = currentSituation.correctAltitude(cg, true);
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
                log.change = m_currentSituationChange;
                log.usedSetup = m_currentSetup;
                log.elevationInfo = elv.arg(elvStats.first).arg(elvStats.second);
                log.cgAboveGround = cg;
                log.sceneryOffset = m_currentSceneryOffset;
                m_logger->logInterpolation(log);
            }

            // bye
            m_lastInterpolation = currentSituation;
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
        CAircraftParts CInterpolator<Derived>::getInterpolatedOrGuessedParts()
        {
            CAircraftParts parts;
            if (m_currentSetup.isAircraftPartsEnabled()) { parts = this->getInterpolatedParts(); }
            if (!m_currentPartsStatus.isSupportingParts())
            {
                // check if model has been thru model matching
                parts.guessParts(m_lastInterpolation, m_currentSituationChange, m_model);
            }
            this->logParts(parts, 0, false);
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
                   boolToYesNo(m_lastInterpolation.isNull());
        }

        template<typename Derived>
        void CInterpolator<Derived>::resetLastInterpolation()
        {
            m_lastInterpolation.setNull();
        }

        template<typename Derived>
        void CInterpolator<Derived>::clear()
        {
            this->resetLastInterpolation();
            m_model = CAircraftModel();
            m_currentSceneryOffset = CLength::null();
            m_currentSituationChange = CAircraftSituationChange::null();
            m_currentSituations.clear();
            m_currentTimeMsSinceEpoch = -1;
            m_situationsLastModified = -1;
            m_situationsLastModifiedUsed = -1;
            m_currentInterpolationStatus.reset();
            m_currentPartsStatus.reset();
            m_interpolatedSituationsCounter = 0;
        }

        template<typename Derived>
        bool CInterpolator<Derived>::initIniterpolationStepData(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup)
        {
            Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

            const bool slowUpdates = ((m_interpolatedSituationsCounter % 25) == 0);
            m_currentTimeMsSinceEpoch = currentTimeSinceEpoc;
            m_situationsLastModified = this->situationsLastModified(m_callsign);
            m_currentSetup = setup;
            m_currentSituations = this->remoteAircraftSituationsAndChange(true);
            m_currentInterpolationStatus.reset();
            m_currentPartsStatus.reset();

            if (!m_model.hasCG() || slowUpdates)
            {
                this->getAndFetchModelCG(); // update CG
            }

            m_currentInterpolationStatus.setSituationsCount(m_currentSituations.size());
            if (m_currentSituations.isEmpty())
            {
                m_lastInterpolation = CAircraftSituation::null(); // no interpolation possible for that step
                m_currentInterpolationStatus.setExtraInfo(this->isAircraftInRange(m_callsign) ?
                        QString("No situations, but remote aircraft '%1'").arg(m_callsign.asString()) :
                        QString("Unknown remote aircraft: '%1'").arg(m_callsign.asString()));
            }
            else
            {
                m_interpolatedSituationsCounter++;

                // with the latest updates of T243 the order and the offsets are supposed to be correct
                // so even mixing fast/slow updates shall work
                if (!CBuildConfig::isReleaseBuild())
                {
                    Q_ASSERT_X(m_currentSituations.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Wrong sort order");
                    Q_ASSERT_X(m_currentSituations.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
                }
            }

            return true;
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
            m_situations = -1;
        }

        QString CInterpolationStatus::toQString() const
        {
            return QStringLiteral("Interpolated: ") % boolToYesNo(m_isInterpolated) %
                   QStringLiteral(" | situations: ") % QString::number(m_situations) %
                   QStringLiteral(" | situation valid: ") % boolToYesNo(m_isValidSituation) %
                   (
                       m_extraInfo.isEmpty() ? QStringLiteral("") : QStringLiteral(" info: ") % m_extraInfo
                   );
        }

        bool CPartsStatus::allTrue() const
        {
            return m_supportsParts;
        }

        void CPartsStatus::reset()
        {
            m_supportsParts = false;
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CInterpolator<CInterpolatorLinear>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CInterpolator<CInterpolatorSpline>;
        //! \endcond
    } // namespace
} // namespace
