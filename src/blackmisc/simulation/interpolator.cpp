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
#include <QDateTime>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Simulation
    {
        template <typename Derived>
        CInterpolator<Derived>::CInterpolator(const QString &objectName, const CCallsign &callsign, QObject *parent) :
            QObject(parent),
            m_callsign(callsign)
        {
            this->setObjectName(objectName + " for " + callsign.asString());
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
            status.reset();
            const bool doLogging = this->hasAttachedLogger() && setup.logInterpolation();
            SituationLog log;
            SituationLog *logP = doLogging ? &log : nullptr;

            // any data at all?
            if (m_aircraftSituations.isEmpty()) { return CAircraftSituation(m_callsign); }
            CAircraftSituation currentSituation = m_lastInterpolation.isNull() ? m_aircraftSituations.front() : m_lastInterpolation;
            if (currentSituation.getCallsign() != m_callsign)
            {
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong callsign");
                currentSituation.setCallsign(m_callsign);
            }

            //! \todo KB 2018-03 ground flag refactoring
            // Update current position by hints' elevation
            // * for XP provided by hints.getElevationProvider at current position
            // * for FSX/P3D provided as hints.getElevation which is set to current position of remote aircraft in simulator
            // * As XP uses lazy init we will call getGroundElevation only when needed
            // * default here via getElevationPlane
            // CAltitude currentGroundElevation(hints.getGroundElevation(currentSituation, currentSituation.getDistancePerTime(1000), true, false, logP));
            const CElevationPlane currentGroundElevation = this->findClosestElevationWithinRange(currentSituation, currentSituation.getDistancePerTime(1000));
            currentSituation.setGroundElevationChecked(currentGroundElevation); // set as default

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

            // use derived interpolant function
            currentSituation.setPosition(interpolant.interpolatePosition(setup));
            currentSituation.setAltitude(interpolant.interpolateAltitude(setup));
            currentSituation.setMSecsSinceEpoch(interpolant.getInterpolatedTime());

            // PBH before ground so we can use PBH in guessing ground
            if (setup.isForcingFullInterpolation() || status.isInterpolated())
            {
                const auto pbh = interpolant.pbh();
                currentSituation.setHeading(pbh.getHeading());
                currentSituation.setPitch(pbh.getPitch());
                currentSituation.setBank(pbh.getBank());
                currentSituation.setGroundSpeed(pbh.getGroundSpeed());
                status.setInterpolatedAndCheckSituation(true, currentSituation);
            }

            // Interpolate between altitude and ground elevation, with proportions weighted according to interpolated onGround flag
            constexpr double NoGroundFactor = -1;
            double groundFactor = NoGroundFactor;

            if (setup.isAircraftPartsEnabled())
            {
                // groundFactor = hints.getAircraftParts().isOnGroundInterpolated();
                if (groundFactor > 0.0)
                {
                    // if not having an ground elevation yet, we fetch from provider (if there is a provider)
                    if (!currentGroundElevation.isNull())
                    {
                        currentGroundElevation = hints.getGroundElevation(currentSituation, true, false, logP); // "expensive on XPlane" if provider is called
                    }

                    if (!currentGroundElevation.isNull())
                    {
                        Q_ASSERT_X(currentGroundElevation.getAltitude().getReferenceDatum() == CAltitude::MeanSeaLevel, Q_FUNC_INFO, "Need MSL value");
                        const CLength cg = this->getCG(m_callsign);
                        const CAltitude groundElevationCG = currentGroundElevation.getAltitude().withOffset(cg);
                        currentSituation.setGroundElevationChecked(currentGroundElevation);
                        // alt = ground + aboveGround * groundFactor
                        //     = ground + (altitude - ground) * groundFactor
                        //     = ground (1 - groundFactor) + altitude * groundFactor
                        currentSituation.setAltitude(CAltitude(currentSituation.getAltitude() * (1.0 - groundFactor) +
                                                               groundElevationCG * groundFactor,
                                                               CAltitude::MeanSeaLevel));
                    }
                }
            }

            // depending on ground factor set ground flag and reliability
            // it will use the hints ground flag or elevation/CG or guessing
            // CInterpolator::setGroundFlagFromInterpolator(hints, groundFactor, currentSituation);

            // we transfer ground elevation for future usage
            if (currentSituation.hasGroundElevation())
            {
                CElevationPlane ep(currentSituation);
                ep.setSinglePointRadius();

                // transfer to newer situations
                log.noTransferredElevations = m_aircraftSituations.setGroundElevationChecked(ep, currentTimeMsSinceEpoc);
            }

            // logging
            if (doLogging)
            {
                log.tsCurrent = currentTimeMsSinceEpoc;
                log.callsign = m_callsign;
                log.groundFactor = groundFactor;
                log.situationCurrent = currentSituation;
                log.usedSetup = setup;
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
        CAircraftParts CInterpolator<Derived>::getInterpolatedParts(qint64 currentTimeMsSinceEpoch,
                const CInterpolationAndRenderingSetupPerCallsign &setup, CPartsStatus &partsStatus, bool log) const
        {
            // (!) this code is used by linear and spline interpolator
            Q_UNUSED(setup);
            partsStatus.reset();
            if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }

            // log for empty parts aircraft parts
            if (m_aircraftParts.isEmpty())
            {
                static const CAircraftParts emptyParts;
                this->logParts(currentTimeMsSinceEpoch, emptyParts, true, log);
                return emptyParts;
            }

            // Parts are supposed to be in correct order, latest first
            const CAircraftPartsList &validParts = m_aircraftParts;

            // stop if we don't have any parts
            if (validParts.isEmpty()) { return {}; }

            partsStatus.setSupportsParts(true);
            CAircraftParts currentParts;
            do
            {
                // find the first parts earlier than the current time
                const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto && p) { return p.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoch; });
                const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
                const auto partsOlder = makeRange(pivot, validParts.end());

                // if (partsOlder.isEmpty()) { currentParts = *(partsNewer.end() - 1); break; }
                if (partsOlder.isEmpty()) { currentParts = *(partsNewer.begin()); break; }
                currentParts = partsOlder.front(); // latest older parts
                if (currentParts.isOnGround()) { break; }

                // here we know aircraft is not on ground, and we check if it was recently on ground or if it will be on ground soon
                const auto latestTakeoff = std::adjacent_find(partsOlder.begin(), partsOlder.end(), [](auto &&, auto && p) { return p.isOnGround(); });
                const auto soonestLanding = std::find_if(partsNewer.begin(), partsNewer.end(), [](auto && p) { return p.isOnGround(); });

                // maxSecs is the maximum effective value of `secondsSinceTakeoff` and `secondsUntilLanding`. If `secondsSinceTakeoff > significantPast` then `takeoffFactor > 1`
                //         and if `secondsUntilLanding > predictableFuture` then `landingFactor > 1`, and `std::min(std::min(takeoffFactor, landingFactor), 1.0)` ensures `>1` is ignored.
                //         but if the offset < 5s then we must use a smaller value for the landing, hence `std::min(max, static_cast<double>(soonestLanding->getTimeOffsetMs()) / 1000.0)`.
                const double maxSecs = 5.0; // preferred length of time over which to blend the onground flag, when possible

                // our clairvoyance is limited by the time offset (all times here in seconds)
                const double significantPastSecs = maxSecs;
                const double predictableFutureSecs = soonestLanding == partsNewer.end() ? maxSecs : std::min(maxSecs, static_cast<double>(soonestLanding->getTimeOffsetMs()) / 1000.0);
                const double secondsSinceTakeoff = latestTakeoff == partsOlder.end() ? maxSecs : (currentTimeMsSinceEpoch - latestTakeoff->getAdjustedMSecsSinceEpoch()) / 1000.0;
                const double secondsUntilLanding = soonestLanding == partsNewer.end() ? maxSecs : (soonestLanding->getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoch) / 1000.0;
                Q_ASSERT(secondsSinceTakeoff >= 0.0);
                Q_ASSERT(secondsUntilLanding >= 0.0);

                //! \fixme In future, will we need to be able to support time offsets of zero?
                BLACK_VERIFY(predictableFutureSecs != 0);
                if (predictableFutureSecs == 0) { break; } // avoid divide by zero

                const double takeoffFactor = secondsSinceTakeoff / significantPastSecs;
                const double landingFactor = secondsUntilLanding / predictableFutureSecs;
                const double airborneFactor = std::min(std::min(takeoffFactor, landingFactor), 1.0);
                currentParts.setOnGroundInterpolated(1.0 - smootherStep(airborneFactor));
            }
            while (false);

            this->logParts(currentTimeMsSinceEpoch, currentParts, false, log);
            return currentParts;
        }

        template<typename Derived>
        void CInterpolator<Derived>::logParts(qint64 timestamp, const CAircraftParts &parts, bool empty, bool log) const
        {
            if (!log || !m_logger) { return; }
            PartsLog logInfo;
            logInfo.callsign = m_callsign;
            logInfo.noNetworkParts = m_aircraftParts.size();
            logInfo.tsCurrent = timestamp;
            logInfo.parts = parts;
            logInfo.empty = empty;
            m_logger->logParts(logInfo);
        }

        template <typename Derived>
        void CInterpolator<Derived>::addAircraftSituation(const CAircraftSituation &situation)
        {
            Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Empty callsign");
            Q_ASSERT_X(situation.getCallsign() == m_callsign, Q_FUNC_INFO, "Wrong callsign");
            if (m_aircraftSituations.isEmpty())
            {
                this->resetLastInterpolation(); // delete any leftover

                // make sure we have enough situations to do start interpolating immediately without waiting for more updates
                // the offsets here (addMSecs) do not really matter
                CAircraftSituation copy(situation);
                copy.addMsecs(-2 * IRemoteAircraftProvider::DefaultOffsetTimeMs);
                m_aircraftSituations.push_frontKeepLatestFirst(copy);
                copy.addMsecs(IRemoteAircraftProvider::DefaultOffsetTimeMs);
                m_aircraftSituations.push_frontKeepLatestFirst(copy);
            }

            // we add new situations at front and keep the latest values (real time) first
            m_aircraftSituations.push_frontKeepLatestFirstAdjustOffset(situation, IRemoteAircraftProvider::MaxSituationsPerCallsign);


            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            Q_ASSERT_X(!m_aircraftSituations.containsZeroOrNegativeOffsetTime(), Q_FUNC_INFO, "Missing offset time");
            Q_ASSERT_X(m_aircraftSituations.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
        }

        template <typename Derived>
        void CInterpolator<Derived>::addAircraftParts(const CAircraftParts &parts, bool adjustZeroOffset)
        {
            const bool adjustOffset = adjustZeroOffset && !parts.hasNonZeroOffsetTime();
            if (adjustOffset)
            {
                const qint64 offset = m_aircraftSituations.isEmpty() ? IRemoteAircraftProvider::DefaultOffsetTimeMs : m_aircraftSituations.front().getTimeOffsetMs();
                CAircraftParts partsCopy(parts);
                partsCopy.setTimeOffsetMs(offset);  // we set the offset of the situation
                CInterpolator<Derived>::addAircraftParts(partsCopy);
                return;
            }

            // here we have an offset
            // unlike situations we do not add parts for spline interpolation
            // this is not needed, as parts do not need 3 values
            Q_ASSERT_X(!adjustZeroOffset || parts.hasNonZeroOffsetTime(), Q_FUNC_INFO, "Missing parts offset");

            // we add new situations at front and keep the latest values (real time) first
            m_aircraftParts.push_frontKeepLatestFirstAdjustOffset(parts, IRemoteAircraftProvider::MaxSituationsPerCallsign);

            // force remote provider to cleanup
            IRemoteAircraftProvider::removeOutdatedParts(m_aircraftParts);

            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            Q_ASSERT_X(adjustZeroOffset ? !m_aircraftParts.containsZeroOrNegativeOffsetTime() : !m_aircraftParts.containsNegativeOffsetTime(), Q_FUNC_INFO, "Missing offset time");
            Q_ASSERT_X(m_aircraftParts.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
        }

        template<typename Derived>
        void CInterpolator<Derived>::addAircraftParts(const CAircraftPartsList &parts, bool adjustZeroOffset)
        {
            for (const CAircraftParts &p : parts)
            {
                this->addAircraftParts(p, adjustZeroOffset);
            }
        }

        template<typename Derived>
        QString CInterpolator<Derived>::getInterpolatorInfo() const
        {
            return QStringLiteral("Callsign: ") %
                   m_callsign.asString() %
                   QStringLiteral(" situations: ") %
                   QString::number(m_aircraftSituations.size()) %
                   QStringLiteral(" parts: ") %
                   QString::number(m_aircraftParts.size()) %
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
            m_aircraftParts.clear();
            m_aircraftSituations.clear();
        }

        template<typename Derived>
        int CInterpolator<Derived>::maxSituations() const
        {
            return IRemoteAircraftProvider::MaxSituationsPerCallsign;
        }

        template<typename Derived>
        int CInterpolator<Derived>::maxParts() const
        {
            return IRemoteAircraftProvider::MaxSituationsPerCallsign;
        }

        template <typename Derived>
        void CInterpolator<Derived>::setGroundFlagFromInterpolator(double groundFactor, CAircraftSituation &situation) const
        {
            // by interpolation
            if (groundFactor >= 1.0)
            {
                situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByInterpolation);
                return;
            }
            if (groundFactor < 1.0 && groundFactor >= 0.0)
            {
                situation.setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByInterpolation);
                return;
            }

            // on elevation and CG
            // remark: to some extend redundant as situation.getCorrectedAltitude() already corrects altitude
            Q_ASSERT_X(!m_callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
            if (situation.hasGroundElevation())
            {
                static const CLength onGroundThresholdLimit(1.0, CLengthUnit::m());
                static const CLength notOnGroundThresholdLimit(10.0, CLengthUnit::m()); // upper boundary
                CLength offset = onGroundThresholdLimit; // very small offset from allowed
                CAircraftSituation::OnGroundDetails reliability = CAircraftSituation::OnGroundByElevation;
                CLength cg = this->getCG(m_callsign);
                if (!cg.isNull())
                {
                    offset += cg;
                    reliability = CAircraftSituation::OnGroundByElevationAndCG;
                }
                else
                {
                    // increase offset a bit
                    offset += CLength(1.0, CLengthUnit::m());
                }

                Q_ASSERT_X(situation.getGroundElevation().getReferenceDatum() == CAltitude::MeanSeaLevel, Q_FUNC_INFO, "Need MSL elevation");
                if (situation.getHeightAboveGround() <= offset)
                {
                    // lower boundary underflow, we can tell we are on ground
                    const CAircraftSituation::IsOnGround og = CAircraftSituation::OnGround;
                    situation.setOnGround(og, reliability);
                    return; // for underflow we can stop here
                }
                else if (situation.getHeightAboveGround() >= notOnGroundThresholdLimit)
                {
                    // upper boundary
                    const CAircraftSituation::IsOnGround og = CAircraftSituation::NotOnGround;
                    situation.setOnGround(og, reliability);
                    return;
                }

                // within an interval were we cannot really tell and continue
            }

            // for VTOL aircraft we give up
            if (this->isVtolAircraft(m_callsign))
            {
                situation.setOnGround(CAircraftSituation::OnGroundSituationUnknown, CAircraftSituation::OnGroundReliabilityNoSet);
                return;
            }

            // we guess on speed, pitch and bank by excluding situations
            situation.setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByGuessing);
            if (qAbs(situation.getPitch().value(CAngleUnit::deg())) > 10) { return; }
            if (qAbs(situation.getBank().value(CAngleUnit::deg())) > 10)  { return; }
            if (situation.getGroundSpeed().value(CSpeedUnit::km_h()) > 50) { return; }

            // not sure, but this is a guess
            situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByGuessing);
        }

        void CInterpolationStatus::setInterpolatedAndCheckSituation(bool succeeded, const CAircraftSituation &situation)
        {
            m_isInterpolated = succeeded;
            this->checkIfValidSituation(situation);
        }

        void CInterpolationStatus::checkIfValidSituation(const CAircraftSituation &situation)
        {
            m_isValidSituation = !situation.isGeodeticHeightNull() && !situation.isPositionNull();
        }

        bool CInterpolationStatus::hasValidInterpolatedSituation() const
        {
            return m_isInterpolated && m_isValidSituation;
        }

        void CInterpolationStatus::reset()
        {
            m_isValidSituation = false;
            m_isInterpolated = false;
        }

        QString CInterpolationStatus::toQString() const
        {
            return QStringLiteral("Interpolated: ") % boolToYesNo(m_isInterpolated) %
                   QStringLiteral(" | situation valid: ") % boolToYesNo(m_isValidSituation);
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


/**
        template <typename Derived>
        void CInterpolator<Derived>::addAircraftParts(const CAircraftParts &parts, bool adjustZeroOffset)
        {
            const bool adjustOffset = adjustZeroOffset && !parts.hasNonZeroOffsetTime();
            if (adjustOffset)
            {
                const qint64 offset = m_aircraftSituations.isEmpty() ? IRemoteAircraftProvider::DefaultOffsetTimeMs : m_aircraftSituations.front().getTimeOffsetMs();
                CAircraftParts partsCopy(parts);
                partsCopy.setTimeOffsetMs(offset);  // we set the offset of the situation
                CInterpolator<Derived>::addAircraftParts(partsCopy);
                return;
            }

            // here we have an offset
            Q_ASSERT_X(!adjustZeroOffset || parts.hasNonZeroOffsetTime(), Q_FUNC_INFO, "Missing parts offset");
            if (m_aircraftParts.isEmpty())
            {
                // make sure we have enough parts to do start interpolating immediately without waiting for more updates
                // the offsets here (addMSecs) do not really matter
                const qint64 minOffset = 100;
                CAircraftParts copy(parts);
                copy.addMsecs(-2 * std::max(parts.getTimeOffsetMs(), minOffset));
                m_aircraftParts.push_frontKeepLatestFirstAdjustOffset(copy);
                copy.addMsecs(parts.getTimeOffsetMs());
                m_aircraftParts.push_frontKeepLatestFirstAdjustOffset(copy);
            }

            // we add new situations at front and keep the latest values (real time) first
            m_aircraftParts.push_frontKeepLatestFirstAdjustOffset(parts, IRemoteAircraftProvider::MaxSituationsPerCallsign);

            // force remote provider to cleanup
            IRemoteAircraftProvider::removeOutdatedParts(m_aircraftParts);

            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            Q_ASSERT_X(adjustZeroOffset ? !m_aircraftParts.containsZeroOrNegativeOffsetTime() : !m_aircraftParts.containsNegativeOffsetTime(), Q_FUNC_INFO, "Missing offset time");
            Q_ASSERT_X(m_aircraftParts.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
        }
**/
