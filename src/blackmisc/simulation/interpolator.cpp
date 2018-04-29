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
                                              IRemoteAircraftProvider *remoteAircraftProvider,
                                              CInterpolationLogger *logger) : m_callsign(callsign)
        {
            // normally when created m_cg is still null since there is no CG in the provider yet

            if (simEnvProvider) { this->setSimulationEnvironmentProvider(simEnvProvider); }
            if (setupProvider)  { this->setInterpolationSetupProvider(setupProvider); }
            if (remoteAircraftProvider)
            {
                this->setRemoteAircraftProvider(remoteAircraftProvider);
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

        template <typename Derived>
        CAircraftSituation CInterpolator<Derived>::getInterpolatedSituation(
            qint64 currentTimeMsSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CInterpolationStatus &status)
        {
            Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

            // this code is used by linear and spline interpolator
            m_interpolatedSituationsCounter++;
            status.reset();
            SituationLog log;
            const bool doLogging = this->hasAttachedLogger() && setup.logInterpolation();

            // any data at all?
            const CAircraftSituationList situations = this->remoteAircraftSituations(m_callsign);
            const int situationsCount = situations.size();
            status.setSituationsCount(situationsCount);
            if (situations.isEmpty())
            {
                status.setExtraInfo(this->isAircraftInRange(m_callsign) ?
                                    QString("No situations, but remote aircraft '%1'").arg(m_callsign.asString()) :
                                    QString("Unknown remote aircraft: '%1'").arg(m_callsign.asString()));
                return CAircraftSituation(m_callsign);
            }

            const CAircraftSituation latest = situations.front();
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
            const CLength cg = latest.hasCG() ? latest.getCG() : this->getAndFetchModelCG();
            currentSituation.setCG(cg);

            // data, split situations by time
            if (currentTimeMsSinceEpoc < 0) { currentTimeMsSinceEpoc = QDateTime::currentMSecsSinceEpoch(); }

            // interpolant function from derived class
            // CInterpolatorLinear::Interpolant or CInterpolatorSpline::Interpolant
            const auto interpolant = derived()->getInterpolant(currentTimeMsSinceEpoc, setup, status, log);

            // succeeded so far?
            if (!status.isInterpolated())
            {
                status.checkIfValidSituation(currentSituation);
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
            status.setInterpolatedAndCheckSituation(true, currentSituation);

            // logging
            if (doLogging)
            {
                static const QString elv("found %1 missed %2");
                const QPair<int, int> elvStats = this->getElevationsFoundMissed();
                log.tsCurrent = currentTimeMsSinceEpoc;
                log.callsign = m_callsign;
                log.groundFactor = currentSituation.getOnGroundFactor();
                log.altCorrection = CAircraftSituation::altitudeCorrectionToString(altCorrection);
                log.situationCurrent = currentSituation;
                log.change = m_situationChange;
                log.usedSetup = setup;
                log.elevationInfo = elv.arg(elvStats.first).arg(elvStats.second);
                log.cgAboveGround = cg;
                log.sceneryOffset = m_currentSceneryOffset;
                m_logger->logInterpolation(log);
            }

            // bye
            m_lastInterpolation = currentSituation;
            return currentSituation;
        }

        CHeading CInterpolatorPbh::getHeading() const
        {
            // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
            // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
            const CHeading headingBegin = m_oldSituation.getHeading();
            CHeading headingEnd = m_newSituation.getHeading();

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
            {
                headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
            {
                headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            return CHeading((headingEnd - headingBegin)
                            * m_simulationTimeFraction
                            + headingBegin,
                            headingBegin.getReferenceNorth());
        }

        CAngle CInterpolatorPbh::getPitch() const
        {
            // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
            const CAngle pitchBegin = m_oldSituation.getPitch();
            const CAngle pitchEnd = m_newSituation.getPitch();
            const CAngle pitch = (pitchEnd - pitchBegin) * m_simulationTimeFraction + pitchBegin;
            return pitch;
        }

        CAngle CInterpolatorPbh::getBank() const
        {
            // Interpolate bank: Bank = (BankB - BankA) * t + BankA
            const CAngle bankBegin = m_oldSituation.getBank();
            const CAngle bankEnd = m_newSituation.getBank();
            const CAngle bank = (bankEnd - bankBegin) * m_simulationTimeFraction + bankBegin;
            return bank;
        }

        CSpeed CInterpolatorPbh::getGroundSpeed() const
        {
            return (m_newSituation.getGroundSpeed() - m_oldSituation.getGroundSpeed())
                   * m_simulationTimeFraction
                   + m_oldSituation.getGroundSpeed();
        }

        template <typename Derived>
        CAircraftParts CInterpolator<Derived>::getInterpolatedParts(
            qint64 currentTimeMsSinceEpoch,
            const CInterpolationAndRenderingSetupPerCallsign &setup, CPartsStatus &partsStatus, bool log) const
        {
            // (!) this code is used by linear and spline interpolator
            Q_UNUSED(setup);
            partsStatus.reset();
            if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }

            // Parts are supposed to be in correct order, latest first
            const CAircraftPartsList validParts = this->remoteAircraftParts(m_callsign);

            // log for empty parts aircraft parts
            if (validParts.isEmpty())
            {
                static const CAircraftParts emptyParts;
                this->logParts(currentTimeMsSinceEpoch, emptyParts, validParts.size(), true, log);
                return emptyParts;
            }

            partsStatus.setSupportsParts(true);
            CAircraftParts currentParts;
            do
            {
                // find the first parts earlier than the current time
                const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto &&p) { return p.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoch; });
                const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
                const auto partsOlder = makeRange(pivot, validParts.end());

                // if (partsOlder.isEmpty()) { currentParts = *(partsNewer.end() - 1); break; }
                if (partsOlder.isEmpty()) { currentParts = *(partsNewer.begin()); break; }
                currentParts = partsOlder.front(); // latest older parts
            }
            while (false);

            this->logParts(currentTimeMsSinceEpoch, currentParts, validParts.size(), false, log);
            return currentParts;
        }

        template<typename Derived>
        CAircraftParts CInterpolator<Derived>::getInterpolatedOrGuessedParts(qint64 currentTimeMsSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, CPartsStatus &partsStatus, bool log) const
        {
            CAircraftParts parts;
            if (setup.isAircraftPartsEnabled()) { parts = this->getInterpolatedParts(currentTimeMsSinceEpoch, setup, partsStatus, log); }
            if (!partsStatus.isSupportingParts())
            {
                // check if model has been thru model matching
                parts.guessParts(this->getLastInterpolatedSituation(), m_situationChange, m_model);
            }
            this->logParts(currentTimeMsSinceEpoch, parts, 0, false, log);
            return parts;
        }

        template<typename Derived>
        void CInterpolator<Derived>::logParts(qint64 timestamp, const CAircraftParts &parts, int partsNo, bool empty, bool log) const
        {
            if (!log || !m_logger) { return; }
            PartsLog logInfo;
            logInfo.callsign = m_callsign;
            logInfo.noNetworkParts = partsNo;
            logInfo.tsCurrent = timestamp;
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
            if (m_model.getCG().isNull())
            {
                const CLength cg(this->getCG(m_callsign));
                if (!cg.isNull()) { m_model.setCG(cg); }
            }
            m_model.setCallsign(m_callsign);
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
