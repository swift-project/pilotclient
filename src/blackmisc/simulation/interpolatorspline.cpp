/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/interpolatorspline.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace
        {
            //! \private https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
            template <size_t N>
            std::array<double, N> solveTridiagonal(std::array<std::array<double, N>, N> &matrix, std::array<double, N> &d)
            {
                // *INDENT-OFF*
                const auto a = [&matrix](auto i) -> double& { return matrix[i][i-1]; }; // subdiagonal
                const auto b = [&matrix](auto i) -> double& { return matrix[i][i  ]; }; // main diagonal
                const auto c = [&matrix](auto i) -> double& { return matrix[i][i+1]; }; // superdiagonal

                // forward sweep
                c(0) /= b(0);
                d[0] /= b(0);
                for (size_t i = 1; i < N; ++i)
                {
                    const double denom = b(i) - a(i) * c(i - 1);
                    if (i < N-1) { c(i) /= denom; }
                    d[i] = (d[i] - a(i) * d[i - 1]) / denom;
                }

                // back substitution
                for (int i = N - 2; i >= 0; --i)
                {
                    d[i] -= c(i) * d[i+1];
                }
                return d;
                // *INDENT-ON*
            }

            //! \private Linear equation expressed as tridiagonal matrix.
            //! https://en.wikipedia.org/wiki/Spline_interpolation
            //! http://blog.ivank.net/interpolation-with-cubic-splines.html
            template <size_t N>
            std::array<double, N> getDerivatives(const std::array<double, N> &x, const std::array<double, N> &y)
            {
                std::array<std::array<double, N>, N> a {{}};
                std::array<double, N> b {{}};

                // *INDENT-OFF*
                a[0][0] = 2.0 / (x[1] - x[0]);
                a[0][1] = 1.0 / (x[1] - x[0]);
                b[0]    = 3.0 * (y[1] - y[0]) / ((x[1] - x[0]) * (x[1] - x[0]));

                a[N-1][N-2] = 1.0 / (x[N-1] - x[N-2]);
                a[N-1][N-1] = 2.0 / (x[N-1] - x[N-2]);
                b[N-1]      = 3.0 * (y[N-1] - y[N-2]) / ((x[N-1] - x[N-2]) * (x[N-1] - x[N-2]));

                for (size_t i = 1; i < N - 1; ++i)
                {
                    a[i][i-1] = 1.0 / (x[i] - x[i-1]);
                    a[i][i  ] = 2.0 / (x[i] - x[i-1]) + 2.0 / (x[i+1] - x[i]);
                    a[i][i+1] = 1.0 / (x[i+1] - x[i]);
                    b[i]      = 3.0 * (y[i] - y[i-1]) / ((x[i] - x[i-1]) * (x[i] - x[i-1]))
                              + 3.0 * (y[i+1] - y[i]) / ((x[i+1] - x[i]) * (x[i+1] - x[i]));
                }
                // *INDENT-ON*

                solveTridiagonal(a, b);
                return b;
            }

            //! \private Cubic interpolation.
            double evalSplineInterval(double x, double x0, double x1, double y0, double y1, double k0, double k1)
            {
                const double t = (x - x0) / (x1 - x0);
                const double a =  k0 * (x1 - x0) - (y1 - y0);
                const double b = -k1 * (x1 - x0) + (y1 - y0);
                const double y = (1 - t) * y0 + t * y1 + t * (1 - t) * (a * (1 - t) + b * t);
                return y;
            }
        }

        CInterpolatorSpline::Interpolant CInterpolatorSpline::getInterpolant(
            qint64 currentTimeMsSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CInterpolationStatus &status,
            SituationLog &log)
        {
            Q_UNUSED(setup);

            // recalculate derivatives only if they changed
            const bool newStep = currentTimeMsSinceEpoc > m_nextSampleAdjustedTime; // new step
            bool recalculate = newStep;
            const qint64 lastModified = this->situationsLastModified(m_callsign);
            if (!recalculate && (lastModified > m_situationsLastModifiedUsed) && this->isAnySituationNearGroundRelevant())
            {
                recalculate = this->areAnyElevationsMissing();
            }

            int situationsSize = -1;
            if (recalculate)
            {
                // with the latest updates of T243 the order and the offsets are supposed to be correct
                // so even mixing fast/slow updates shall work
                const CAircraftSituationList validSituations = this->remoteAircraftSituations(m_callsign);
                situationsSize = validSituations.size();
                m_situationsLastModifiedUsed = lastModified;

                if (!CBuildConfig::isReleaseBuild())
                {
                    Q_ASSERT_X(validSituations.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
                    Q_ASSERT_X(validSituations.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
                }

                // find the first situation earlier than the current time
                const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto &&s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
                const auto situationsNewer = makeRange(validSituations.begin(), pivot);
                const auto situationsOlder = makeRange(pivot, validSituations.end());

                // m_s[0] .. oldest -> m_[2] .. latest
                if (situationsNewer.isEmpty() || situationsOlder.size() < 2) { return m_interpolant; }
                m_s = std::array<CAircraftSituation, 3> {{ *(situationsOlder.begin() + 1), *situationsOlder.begin(), *(situationsNewer.end() - 1) }};

                // we interpolate from 1 -> 2, 0 for smoother interpolation
                if (newStep && !m_lastInterpolation.isNull())
                {
                    m_s[1] = m_lastInterpolation; // true only for the moment we create a new step
                }

                const std::array<std::array<double, 3>, 3> normals {{ m_s[0].getPosition().normalVectorDouble(), m_s[1].getPosition().normalVectorDouble(), m_s[2].getPosition().normalVectorDouble() }};
                PosArray pa;
                pa.x = {{ normals[0][0], normals[1][0], normals[2][0] }}; // oldest -> latest
                pa.y = {{ normals[0][1], normals[1][1], normals[2][1] }};
                pa.z = {{ normals[0][2], normals[1][2], normals[2][2] }};
                pa.t = {{ static_cast<double>(m_s[0].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[1].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[2].getAdjustedMSecsSinceEpoch()) }};

                pa.dx = getDerivatives(pa.t, pa.x);
                pa.dy = getDerivatives(pa.t, pa.y);
                pa.dz = getDerivatives(pa.t, pa.z);

                // - altitude unit must be the same for all three, but the unit itself does not matter
                // - ground elevantion here normally is not available
                // - some info how fast a plane moves: 100km/h => 1sec 27,7m => 5 secs 136m
                // - on an airport the plane does not move very fast, or not at all
                // - and the elevation remains (almost) constant for a wider area
                // - during flying the ground elevation not really matters
                this->updateElevations();
                const double a0 = m_s[0].getCorrectedAltitude(m_cg).value(); // oldest
                const double a1 = m_s[1].getCorrectedAltitude(m_cg).value();
                const double a2 = m_s[2].getCorrectedAltitude(m_cg).value(); // latest
                pa.a = {{ a0, a1, a2 }};
                pa.gnd = {{ m_s[0].getOnGroundFactor(), m_s[1].getOnGroundFactor(), m_s[2].getOnGroundFactor() }};
                pa.da = getDerivatives(pa.t, pa.a);
                pa.dgnd = getDerivatives(pa.t, pa.gnd);
                Q_ASSERT_X(this->areAltitudeUnitsSame(), Q_FUNC_INFO, "Altitude unit mismatch");

                // m_prevSampleAdjustedTime = situationsOlder.begin()->getAdjustedMSecsSinceEpoch(); // m_s[1]
                // m_nextSampleAdjustedTime = (situationsNewer.end() - 1)->getAdjustedMSecsSinceEpoch(); // m_s[2]
                // m_prevSampleTime = situationsOlder.begin()->getMSecsSinceEpoch(); // m_s[1]
                // m_nextSampleTime = (situationsNewer.end() - 1)->getMSecsSinceEpoch(); // m_s[2]
                // m_interpolant = Interpolant(pa, situationsOlder.begin()->getAltitude().getUnit(), { *situationsOlder.begin(), *(situationsNewer.end() - 1) });

                m_prevSampleAdjustedTime = m_s[1].getAdjustedMSecsSinceEpoch();
                m_nextSampleAdjustedTime = m_s[2].getAdjustedMSecsSinceEpoch(); // latest
                m_prevSampleTime = m_s[1].getMSecsSinceEpoch();
                m_nextSampleTime = m_s[2].getMSecsSinceEpoch(); // latest
                m_interpolant = Interpolant(pa, m_s[2].getAltitudeUnit(), CInterpolatorPbh(m_s[1], m_s[2]));
                Q_ASSERT_X(m_prevSampleAdjustedTime < m_nextSampleAdjustedTime, Q_FUNC_INFO, "Wrong time order");

                // VERIFY
                this->verifyInterpolationSituations(m_s[0], m_s[1], m_s[2], setup); // oldest -> latest
            }

            // Example:
            // prev.sample time 5 (received at 0) , next sample time 10 (received at 5)
            // cur.time 6: dt1=6-5=1, dt2=5 => fraction 1/5
            // cur.time 9: dt1=9-5=4, dt2=5 => fraction 4/5
            //
            // we use different offset times for interim pos. updates
            // prev.sample time 5 (received at 0) , 7/r:5, 10 (rec. at 5)
            // cur.time 6: dt1=6-5=1, dt2=7-5 => fraction 1/2
            // cur.time 9: dt1=9-7=2, dt2=10-7=3 => fraction 2/3
            // we use different offset times for fast pos. updates
            const double dt1 = static_cast<double>(currentTimeMsSinceEpoc - m_prevSampleAdjustedTime);
            const double dt2 = static_cast<double>(m_nextSampleAdjustedTime - m_prevSampleAdjustedTime);
            const double timeFraction = dt1 / dt2;

            // is that correct with dt2, or would it be
            // m_nextSampleTime - m_prevSampleTime
            // as long as the offset time is constant, it does not matter
            const qint64 interpolatedTime = m_prevSampleTime + timeFraction * dt2;

            // time fraction is expected between 0-1
            status.setInterpolated(true);
            m_interpolant.setTimes(currentTimeMsSinceEpoc, timeFraction, interpolatedTime);

            if (this->hasAttachedLogger() && setup.logInterpolation())
            {
                if (situationsSize < 0) { situationsSize = this->remoteAircraftSituationsCount(m_callsign); }
                log.interpolationSituations.push_back(m_s[0]);
                log.interpolationSituations.push_back(m_s[1]);
                log.interpolationSituations.push_back(m_s[2]); // latest at end
                log.interpolator = 's';
                log.deltaSampleTimesMs = dt2;
                log.simTimeFraction = timeFraction;
                log.noNetworkSituations = situationsSize;
                log.tsInterpolated = interpolatedTime; // without offsets
            }

            return m_interpolant;
        }

        bool CInterpolatorSpline::updateElevations()
        {
            bool updated = false;
            for (unsigned int i = 0; i < m_s.size(); i++)
            {
                if (m_s[i].hasGroundElevation()) { continue; } // do not override existing values
                const CElevationPlane plane = this->findClosestElevationWithinRange(m_s[i], CElevationPlane::singlePointRadius());
                const bool u = m_s[i].setGroundElevationChecked(plane);
                updated |= u;
            }
            return updated;
        }

        bool CInterpolatorSpline::areAnyElevationsMissing() const
        {
            for (unsigned int i = 0; i < m_s.size(); i++)
            {
                if (!m_s[i].hasGroundElevation()) { return true; }
            }
            return false;
        }

        bool CInterpolatorSpline::isAnySituationNearGroundRelevant() const
        {
            for (unsigned int i = 0; i < m_s.size(); i++)
            {
                if (!m_s[i].canLikelySkipNearGroundInterpolation()) { return true; }
            }
            return false;
        }

        bool CInterpolatorSpline::areAltitudeUnitsSame(const CLengthUnit &compare) const
        {
            if (m_s.size() < 1) { return true; }
            const CLengthUnit c = compare.isNull() ? m_s[0].getAltitudeUnit() : compare;
            for (unsigned int i = 0; i < m_s.size(); i++)
            {
                if (m_s[i].getAltitudeUnit() != c) { return false; }
            }
            return true;
        }

        CAircraftSituation CInterpolatorSpline::Interpolant::interpolatePositionAndAltitude(const CAircraftSituation &currentSituation, bool interpolateGndFactor) const
        {
            const double t1 = m_pa.t[1];
            const double t2 = m_pa.t[2];
            const double newX = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.x[1], m_pa.x[2], m_pa.dx[1], m_pa.dx[2]);
            const double newY = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.y[1], m_pa.y[2], m_pa.dy[1], m_pa.dy[2]);
            const double newZ = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.z[1], m_pa.z[2], m_pa.dz[1], m_pa.dz[2]);

            CAircraftSituation newSituation(currentSituation);
            const std::array<double, 3> normalVector = {{ newX, newY, newZ }};
            const CCoordinateGeodetic currentPosition(normalVector);

            const double newA = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.a[1], m_pa.a[2], m_pa.da[1], m_pa.da[2]);
            const CAltitude alt(newA, m_altitudeUnit);

            newSituation.setPosition(currentPosition);
            newSituation.setAltitude(alt);
            newSituation.setMSecsSinceEpoch(this->getInterpolatedTime());

            if (interpolateGndFactor)
            {
                const double gnd1 = m_pa.gnd[1];
                const double gnd2 = m_pa.gnd[2];
                do
                {
                    newSituation.setOnGroundDetails(CAircraftSituation::OnGroundByInterpolation);
                    if (gfEqualAirborne(gnd1, gnd2)) { newSituation.setOnGround(false); break; }
                    if (gfEqualOnGround(gnd1, gnd2)) { newSituation.setOnGround(true); break; }
                    const double newGnd = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, gnd1, gnd2, m_pa.dgnd[1], m_pa.dgnd[2]);
                    newSituation.setOnGroundFactor(newGnd);
                    newSituation.setOnGroundFromGroundFactorFromInterpolation();
                }
                while (false);
            }
            return newSituation;
        }

        void CInterpolatorSpline::Interpolant::setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs)
        {
            m_currentTimeMsSinceEpoc = currentTimeMs;
            m_interpolatedTime = interpolatedTimeMs;
            m_pbh.setTimeFraction(timeFraction);
        }

        void CInterpolatorSpline::PosArray::initToZero()
        {
            for (int i = 0; i < 3; i++)
            {
                x[i]  = 0; y[i] = 0; z[i] = 0;
                a[i]  = 0; t[i] = 0;
                dx[i] = 0; dy[i] = 0; dz[i] = 0;
                da[i] = 0;
                gnd[i] = 0; dgnd[i] = 0;
            }
        }

        const CInterpolatorSpline::PosArray &CInterpolatorSpline::PosArray::zeroPosArray()
        {
            static const PosArray pa = []
            {
                PosArray p;
                p.initToZero();
                return p;
            }();
            return pa;
        }
    } // ns
} // ns
