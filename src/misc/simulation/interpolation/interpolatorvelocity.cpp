// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/interpolation/interpolatorvelocity.h"

#include <algorithm>
#include <cmath>

#include "misc/range.h"

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace swift::misc::simulation
{
    namespace
    {
        //! Convergence time constant of the error correction [s]
        constexpr double c_correctionTimeConstantSec = 0.7;

        //! Max.extrapolated time from the anchor [s], avoids runaway with missing updates
        constexpr double c_maxExtrapolationSec = 5.0;

        //! Max.backwards extrapolation [s]
        constexpr double c_minExtrapolationSec = -1.0;

        //! Corrections beyond that are treated as garbage (teleport etc.), capped
        constexpr double c_maxCorrectionMeters = 250.0;
        constexpr double c_maxCorrectionDegrees = 15.0;

        //! Below that a velocity is considered zero (parked aircraft)
        constexpr double c_minLinearVelocityM_s = 0.0005;
        constexpr double c_minAngularVelocityRad_s = 0.0001;

        //! Sanity limit, velocities beyond are garbage
        constexpr double c_maxLinearVelocityM_s = 2000.0;

        //! WGS84 parameters for the local flat earth approximation
        constexpr double c_semiMajorM = 6378137.0;
        constexpr double c_squaredEccentricity = 6.69437999014e-3;
        constexpr double c_radToDeg = 57.29577951308232;
        constexpr double c_degToRad = 0.017453292519943295;

        bool hasSignificantVelocity(const CAircraftVelocity &velocity)
        {
            const double v =
                std::sqrt(velocity.getVelocityX(CSpeedUnit::m_s()) * velocity.getVelocityX(CSpeedUnit::m_s()) +
                          velocity.getVelocityY(CSpeedUnit::m_s()) * velocity.getVelocityY(CSpeedUnit::m_s()) +
                          velocity.getVelocityZ(CSpeedUnit::m_s()) * velocity.getVelocityZ(CSpeedUnit::m_s()));
            if (v > c_minLinearVelocityM_s && v < c_maxLinearVelocityM_s) { return true; }
            return std::abs(velocity.getPitchVelocity(CAngleUnit::rad(), CTimeUnit::s())) > c_minAngularVelocityRad_s ||
                   std::abs(velocity.getRollVelocity(CAngleUnit::rad(), CTimeUnit::s())) > c_minAngularVelocityRad_s ||
                   std::abs(velocity.getHeadingVelocity(CAngleUnit::rad(), CTimeUnit::s())) > c_minAngularVelocityRad_s;
        }

        //! Normalize to [0, 360)
        double normalizedDegrees(double degrees)
        {
            double d = std::fmod(degrees, 360.0);
            if (d < 0.0) { d += 360.0; }
            return d;
        }

        //! Normalize to [-180, 180)
        double centeredDegrees(double degrees)
        {
            double d = std::fmod(degrees + 540.0, 360.0) - 180.0;
            if (d < -180.0) { d += 360.0; }
            return d;
        }

        double clampValue(double value, double min, double max)
        {
            return (value < min) ? min : (value > max) ? max : value;
        }

        //! Local meridian / transverse earth radii [m] at the given latitude [deg]
        std::pair<double, double> localEarthRadiiM(double latitudeDeg)
        {
            const double sinLat = std::sin(latitudeDeg * c_degToRad);
            const double radiusMeridianM = c_semiMajorM * (1.0 - c_squaredEccentricity) /
                                           std::pow(1.0 - c_squaredEccentricity * sinLat * sinLat, 1.5);
            const double radiusTransverseM = c_semiMajorM / std::sqrt(1.0 - c_squaredEccentricity * sinLat * sinLat);
            return { radiusMeridianM, radiusTransverseM };
        }
    } // namespace

    CInterpolatorVelocity::CInterpolant::CInterpolant(const CAircraftSituation &situation, const CHeading &heading,
                                                      const CAngle &pitch, const CAngle &bank,
                                                      const CSpeed &groundspeed, qint64 interpolatedTime)
        : IInterpolant(interpolatedTime), m_situation(situation), m_pbh(situation, heading, pitch, bank, groundspeed)
    {}

    std::tuple<CCoordinateGeodetic, CAltitude>
    CInterpolatorVelocity::CInterpolant::interpolatePositionAndAltitude() const
    {
        return { m_situation.getPosition(), m_situation.getAltitude() };
    }

    COnGroundInfo CInterpolatorVelocity::CInterpolant::interpolateGroundFactor() const
    {
        return m_situation.getOnGroundInfo();
    }

    void CInterpolatorVelocity::updateAnchorAndCorrection()
    {
        if (m_currentSituations.isEmpty()) { return; }

        // situations are sorted latest first
        const CAircraftSituation newest = m_currentSituations.front();

        // latest situation which carries a usable velocity, it becomes the new prediction anchor
        CAircraftSituation newAnchor;
        for (const CAircraftSituation &s : m_currentSituations)
        {
            if (s.hasVelocity() && hasSignificantVelocity(s.getVelocity()))
            {
                newAnchor = s;
                break;
            }
        }

        if (newAnchor.isNull() || !newAnchor.hasVelocity())
        {
            // no velocity data at all, fallback interpolation will be used
            m_anchor.setNull();
            m_corrCurr = {};
            m_corrPrev = {};
            return;
        }

        if (m_anchor.isNull() || !m_corrCurr.isValid())
        {
            // first fix: start with a clean correction state
            m_anchor = newAnchor;
            m_corrCurr = {};
            m_corrCurr.timeMs = newest.getAdjustedMSecsSinceEpoch();
            m_corrPrev = {};
            return;
        }

        const qint64 newestAdjustedMs = newest.getAdjustedMSecsSinceEpoch();
        if (newestAdjustedMs <= m_corrCurr.timeMs)
        {
            // no time progress or time went backwards, keep the correction segments
            m_anchor = newAnchor;
            return;
        }

        // where would the renderer have shown the aircraft at the time of the newest fix?
        const SExtrapolated predicted = this->extrapolateFrom(m_anchor, newestAdjustedMs);

        // total error between shown and actual, converged smoothly from here on (no snap)
        const double sinLat = std::sin(predicted.latitudeDeg * c_degToRad);
        const double cosLat = std::cos(predicted.latitudeDeg * c_degToRad);
        const double radiusTransverseM = c_semiMajorM / std::sqrt(1.0 - c_squaredEccentricity * sinLat * sinLat);

        const double dLatDeg = newest.latitude().value(CAngleUnit::deg()) - predicted.latitudeDeg;
        const double dLonDeg = newest.longitude().value(CAngleUnit::deg()) - predicted.longitudeDeg;
        const double dNorthM = dLatDeg * localEarthRadiiM(predicted.latitudeDeg).first / c_radToDeg;
        const double dEastM = dLonDeg * radiusTransverseM * cosLat / c_radToDeg;
        const double dUpM = newest.getAltitude().value(CLengthUnit::m()) - predicted.altitudeM;

        SCorrectionSegment segment;
        segment.timeMs = newestAdjustedMs;
        segment.eastM = clampValue(-dEastM, -c_maxCorrectionMeters, c_maxCorrectionMeters);
        segment.northM = clampValue(-dNorthM, -c_maxCorrectionMeters, c_maxCorrectionMeters);
        segment.upM = clampValue(-dUpM, -c_maxCorrectionMeters, c_maxCorrectionMeters);
        segment.pitchDeg = clampValue(-centeredDegrees(newest.getPitch().value(CAngleUnit::deg()) - predicted.pitchDeg),
                                      -c_maxCorrectionDegrees, c_maxCorrectionDegrees);
        segment.bankDeg = clampValue(-centeredDegrees(newest.getBank().value(CAngleUnit::deg()) - predicted.bankDeg),
                                     -c_maxCorrectionDegrees, c_maxCorrectionDegrees);
        segment.headingDeg =
            clampValue(-centeredDegrees(newest.getHeading().value(CAngleUnit::deg()) - predicted.headingDeg),
                       -c_maxCorrectionDegrees, c_maxCorrectionDegrees);

        m_corrPrev = m_corrCurr;
        m_corrCurr = segment;
        m_anchor = newAnchor;
    }

    void CInterpolatorVelocity::correctionAt(qint64 adjustedTimeMs, const SCorrectionSegment **segment,
                                             double *factor) const
    {
        static const SCorrectionSegment zero;
        if (!m_corrCurr.isValid())
        {
            *segment = &zero;
            *factor = 0.0;
            return;
        }

        // renders after the establishment time use the current segment, renders in between the previous
        // and the current establishment time use the previous segment (no pre-warped positions)
        if (adjustedTimeMs >= m_corrCurr.timeMs)
        {
            *segment = &m_corrCurr;
            *factor = clampValue(std::exp(-(adjustedTimeMs - m_corrCurr.timeMs) / 1000.0 / c_correctionTimeConstantSec),
                                 0.0, 1.5);
        }
        else if (m_corrPrev.isValid() && adjustedTimeMs >= m_corrPrev.timeMs)
        {
            *segment = &m_corrPrev;
            *factor = clampValue(std::exp(-(adjustedTimeMs - m_corrPrev.timeMs) / 1000.0 / c_correctionTimeConstantSec),
                                 0.0, 1.5);
        }
        else
        {
            // render time before all known segments (startup), hold previous value
            *segment = m_corrPrev.isValid() ? &m_corrPrev : &zero;
            *factor = m_corrPrev.isValid() ? 1.0 : 0.0;
        }
    }

    CInterpolatorVelocity::SExtrapolated CInterpolatorVelocity::extrapolateFrom(const CAircraftSituation &anchor,
                                                                                qint64 adjustedTimeMs) const
    {
        Q_ASSERT_X(!anchor.isNull(), Q_FUNC_INFO, "No anchor");
        const CAircraftVelocity velocity = anchor.getVelocity();

        double dtSec = (adjustedTimeMs - anchor.getAdjustedMSecsSinceEpoch()) / 1000.0;
        dtSec = clampValue(dtSec, c_minExtrapolationSec, c_maxExtrapolationSec);

        const SCorrectionSegment *segment = nullptr;
        double factor = 0.0;
        this->correctionAt(adjustedTimeMs, &segment, &factor);

        const double dEastM = velocity.getVelocityX(CSpeedUnit::m_s()) * dtSec + segment->eastM * factor;
        const double dNorthM = velocity.getVelocityZ(CSpeedUnit::m_s()) * dtSec + segment->northM * factor;
        const double dUpM = anchor.isOnGround() ?
                                segment->upM * factor :
                                velocity.getVelocityY(CSpeedUnit::m_s()) * dtSec + segment->upM * factor;

        const double anchorLatDeg = anchor.latitude().value(CAngleUnit::deg());
        const double anchorLonDeg = anchor.longitude().value(CAngleUnit::deg());
        const auto radii = localEarthRadiiM(anchorLatDeg);
        const double cosLat = std::cos(anchorLatDeg * c_degToRad);

        SExtrapolated e;
        e.latitudeDeg = anchorLatDeg + dNorthM / radii.first * c_radToDeg;
        e.longitudeDeg = anchorLonDeg + dEastM / (radii.second * cosLat) * c_radToDeg;
        e.altitudeM = anchor.getAltitude().value(CLengthUnit::m()) + dUpM;
        e.pitchDeg = anchor.getPitch().value(CAngleUnit::deg()) +
                     velocity.getPitchVelocity(CAngleUnit::rad(), CTimeUnit::s()) * dtSec * c_radToDeg +
                     segment->pitchDeg * factor;
        e.bankDeg = anchor.getBank().value(CAngleUnit::deg()) +
                    velocity.getRollVelocity(CAngleUnit::rad(), CTimeUnit::s()) * dtSec * c_radToDeg +
                    segment->bankDeg * factor;
        e.headingDeg =
            normalizedDegrees(anchor.getHeading().value(CAngleUnit::deg()) +
                              velocity.getHeadingVelocity(CAngleUnit::rad(), CTimeUnit::s()) * dtSec * c_radToDeg +
                              segment->headingDeg * factor);
        e.realTimeMs = anchor.getMSecsSinceEpoch() + qRound(dtSec * 1000.0);
        return e;
    }

    bool CInterpolatorVelocity::hasVelocityData() const
    {
        // check the latest situations, in case the latest one is a plain position update without velocity
        constexpr int c_checkLatestNSituations = 5;
        for (int i = 0; i < c_checkLatestNSituations; i++)
        {
            const CAircraftSituation s = this->remoteAircraftSituation(m_callsign, i);
            if (s.isNull()) { break; }
            if (s.hasVelocity() && hasSignificantVelocity(s.getVelocity())) { return true; }
        }
        return false;
    }

    const IInterpolant &CInterpolatorVelocity::getFallbackInterpolant(SituationLog &log)
    {
        // bracket the current time like the linear interpolator, situations sorted latest first
        const auto pivot =
            std::partition_point(m_currentSituations.begin(), m_currentSituations.end(), [=, this](auto &&s) {
                return s.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch;
            });
        const auto situationsNewer = makeRange(m_currentSituations.begin(), pivot);
        const auto situationsOlder = makeRange(pivot, m_currentSituations.end());

        CAircraftSituation startSituation;
        CAircraftSituation endSituation;
        bool hold = true;

        if (situationsOlder.isEmpty() && !situationsNewer.isEmpty())
        {
            startSituation = *(situationsNewer.end() - 1); // hold oldest of newer
        }
        else if (situationsNewer.isEmpty())
        {
            if (situationsOlder.size() < 2)
            {
                startSituation = situationsOlder.front(); // hold latest older
            }
            else
            {
                startSituation = *(situationsOlder.begin() + 1);
                endSituation = situationsOlder.front();
                hold = false;
            }
        }
        else
        {
            startSituation = situationsOlder.front();
            endSituation = *(situationsNewer.end() - 1);
            hold = false;
        }

        if (hold)
        {
            m_currentInterpolationStatus.setInterpolatedAndCheckSituation(false, startSituation);
            m_fallbackInterpolant = CInterpolatorLinear::CInterpolant(startSituation);
            log.interpolator = 'f';
            return m_fallbackInterpolant;
        }

        const qint64 sampleDeltaTimeMs =
            endSituation.getAdjustedMSecsSinceEpoch() - startSituation.getAdjustedMSecsSinceEpoch();
        if (sampleDeltaTimeMs <= 0)
        {
            m_currentInterpolationStatus.setInterpolatedAndCheckSituation(false, endSituation);
            m_fallbackInterpolant = CInterpolatorLinear::CInterpolant(endSituation);
            log.interpolator = 'f';
            return m_fallbackInterpolant;
        }

        const double distanceToSplitTimeMs = endSituation.getAdjustedMSecsSinceEpoch() - m_currentTimeMsSinceEpoch;
        const double simulationTimeFraction = clampValue(1.0 - (distanceToSplitTimeMs / sampleDeltaTimeMs), 0.0, 1.0);
        const qint64 interpolatedTime =
            startSituation.getMSecsSinceEpoch() + qRound(sampleDeltaTimeMs * simulationTimeFraction);

        CAircraftSituation currentSituation(startSituation);
        currentSituation.setMSecsSinceEpoch(interpolatedTime);
        m_currentInterpolationStatus.setInterpolatedAndCheckSituation(true, currentSituation);
        m_fallbackInterpolant =
            CInterpolatorLinear::CInterpolant(startSituation, endSituation, simulationTimeFraction, interpolatedTime);
        log.interpolator = 'f';
        return m_fallbackInterpolant;
    }

    const IInterpolant &CInterpolatorVelocity::getInterpolant(SituationLog &log)
    {
        const bool updated = m_situationsLastModifiedUsed < m_situationsLastModified;
        if (updated)
        {
            m_situationsLastModifiedUsed = m_situationsLastModified;
            this->updateAnchorAndCorrection();
        }

        // render anchor: latest situation with usable velocity at or before the render time,
        // situations sorted latest first
        const auto pivot =
            std::partition_point(m_currentSituations.begin(), m_currentSituations.end(), [=, this](auto &&s) {
                return s.getAdjustedMSecsSinceEpoch() > m_currentTimeMsSinceEpoch;
            });
        const auto situationsOlder = makeRange(pivot, m_currentSituations.end());

        CAircraftSituation renderAnchor;
        for (const CAircraftSituation &s : situationsOlder)
        {
            if (s.hasVelocity() && hasSignificantVelocity(s.getVelocity()))
            {
                renderAnchor = s;
                break;
            }
        }

        if (renderAnchor.isNull() || !renderAnchor.hasVelocity() || m_corrCurr.isValid() == false)
        {
            // no velocity data (e.g. non VATSIM velocity network), use linear interpolation
            return this->getFallbackInterpolant(log);
        }

        // extrapolate for the current render time, cheap math, always recalculated
        const SExtrapolated extrapolated = this->extrapolateFrom(renderAnchor, m_currentTimeMsSinceEpoch);

        CAircraftSituation situation(renderAnchor); // copies callsign, ground info, elevation
        situation.setPosition(
            CCoordinateGeodetic(CLatitude(extrapolated.latitudeDeg, CAngleUnit::deg()),
                                CLongitude(extrapolated.longitudeDeg, CAngleUnit::deg()),
                                CAltitude(extrapolated.altitudeM, CAltitude::MeanSeaLevel, CLengthUnit::m())));
        situation.setHeading(CHeading(normalizedDegrees(extrapolated.headingDeg), CHeading::True, CAngleUnit::deg()));
        situation.setPitch(CAngle(extrapolated.pitchDeg, CAngleUnit::deg()));
        situation.setBank(CAngle(extrapolated.bankDeg, CAngleUnit::deg()));

        CSpeed groundspeed = renderAnchor.getGroundSpeed();
        if (groundspeed.isNull())
        {
            const CAircraftVelocity velocity = renderAnchor.getVelocity();
            const double gs =
                std::hypot(velocity.getVelocityX(CSpeedUnit::m_s()), velocity.getVelocityZ(CSpeedUnit::m_s()));
            groundspeed = CSpeed(gs, CSpeedUnit::m_s());
        }

        m_currentInterpolationStatus.setInterpolatedAndCheckSituation(true, situation);
        m_interpolant = CInterpolant(situation, situation.getHeading(), situation.getPitch(), situation.getBank(),
                                     groundspeed, extrapolated.realTimeMs);
        log.interpolator = 'v';
        return m_interpolant;
    }

    void CInterpolatorVelocity::anchor() {}
} // namespace swift::misc::simulation
