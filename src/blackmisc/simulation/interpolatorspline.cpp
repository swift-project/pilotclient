/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/interpolatorspline.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/logmessage.h"

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

        CInterpolatorSpline::Interpolant CInterpolatorSpline::getInterpolant(qint64 currentTimeMsSinceEpoc,
                const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status,
                CInterpolationLogger::SituationLog &log)
        {
            Q_UNUSED(hints);
            Q_UNUSED(setup);

            // recalculate derivatives only if they changed
            if (currentTimeMsSinceEpoc > m_nextSampleTime)
            {
                // find the first situation not in the correct order, keep only the situations before that one
                const auto end = std::is_sorted_until(m_aircraftSituations.begin(), m_aircraftSituations.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
                const auto validSituations = makeRange(m_aircraftSituations.begin(), end);

                // find the first situation earlier than the current time
                const auto pivot = std::partition_point(validSituations.begin(), validSituations.end(), [ = ](auto && s) { return s.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoc; });
                const auto situationsNewer = makeRange(validSituations.begin(), pivot);
                const auto situationsOlder = makeRange(pivot, validSituations.end());

                if (situationsNewer.isEmpty() || situationsOlder.size() < 2)
                {
                    return m_interpolant;
                }

                m_s = std::array<CAircraftSituation, 3> {{ *(situationsOlder.begin() + 1), *situationsOlder.begin(), *(situationsNewer.end() - 1) }};

                // - altitude unit must be the same for all three, but the unit itself does not matter
                // - ground elevantion here normally is not available
                // - only use elevation plane here, do not call provider
                // - some info how has a plane moves: 100km/h => 1sec 27,7m => 5 secs 136m
                // - on an airport the plane does not move very fast, or not at all
                // - and the elevation remains (almost) constant for a wider area
                // - flying the ground elevation not really matters
                if (!hints.getElevationPlane().isNull())
                {
                    // do not override existing values
                    m_s[0].setGroundElevationChecked(hints.getElevationPlane());
                    m_s[1].setGroundElevationChecked(hints.getElevationPlane());
                    m_s[2].setGroundElevationChecked(hints.getElevationPlane());
                }

                const double a0 = m_s[0].getCorrectedAltitude(hints.getCGAboveGround()).value();
                const double a1 = m_s[1].getCorrectedAltitude(hints.getCGAboveGround()).value();
                const double a2 = m_s[2].getCorrectedAltitude(hints.getCGAboveGround()).value();

                const std::array<std::array<double, 3>, 3> normals {{ m_s[0].getPosition().normalVectorDouble(), m_s[1].getPosition().normalVectorDouble(), m_s[2].getPosition().normalVectorDouble() }};
                PosArray pa;
                pa.x = {{ normals[0][0], normals[1][0], normals[2][0] }};
                pa.y = {{ normals[0][1], normals[1][1], normals[2][1] }};
                pa.z = {{ normals[0][2], normals[1][2], normals[2][2] }};
                pa.a = {{ a0, a1, a2 }};
                pa.t = {{ static_cast<double>(m_s[0].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[1].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[2].getAdjustedMSecsSinceEpoch()) }};

                pa.dx = getDerivatives(pa.t, pa.x);
                pa.dy = getDerivatives(pa.t, pa.y);
                pa.dz = getDerivatives(pa.t, pa.z);
                pa.da = getDerivatives(pa.t, pa.a);

                m_prevSampleTime = situationsOlder.begin()->getAdjustedMSecsSinceEpoch();
                m_nextSampleTime = (situationsNewer.end() - 1)->getAdjustedMSecsSinceEpoch();
                m_interpolant = Interpolant(pa, situationsOlder.begin()->getAltitude().getUnit(), { *situationsOlder.begin(), *(situationsNewer.end() - 1) });
            }

            const double dt1 = static_cast<double>(currentTimeMsSinceEpoc - m_prevSampleTime);
            const double dt2 = static_cast<double>(m_nextSampleTime - m_prevSampleTime);
            const double timeFraction = dt1 / dt2;

            if (this->hasAttachedLogger())
            {
                log.interpolationSituations.push_back(m_s[0]);
                log.interpolationSituations.push_back(m_s[1]);
                log.interpolationSituations.push_back(m_s[2]); // latest at end
                log.interpolator = 's';
                log.deltaSampleTimesMs = dt2;
                log.simulationTimeFraction = timeFraction;
                log.tsInterpolated = log.newestInterpolationSituation().getMSecsSinceEpoch(); // without offset
            }

            status.setInterpolated(true);
            m_interpolant.setTimes(currentTimeMsSinceEpoc, timeFraction);
            return m_interpolant;
        }

        CCoordinateGeodetic CInterpolatorSpline::Interpolant::interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            const double newX = evalSplineInterval(m_currentTimeMsSinceEpoc, m_pa.t[1], m_pa.t[2], m_pa.x[1], m_pa.x[2], m_pa.dx[1], m_pa.dx[2]);
            const double newY = evalSplineInterval(m_currentTimeMsSinceEpoc, m_pa.t[1], m_pa.t[2], m_pa.y[1], m_pa.y[2], m_pa.dy[1], m_pa.dy[2]);
            const double newZ = evalSplineInterval(m_currentTimeMsSinceEpoc, m_pa.t[1], m_pa.t[2], m_pa.z[1], m_pa.z[2], m_pa.dz[1], m_pa.dz[2]);

            CCoordinateGeodetic currentPosition;
            currentPosition.setNormalVector(newX, newY, newZ);
            return currentPosition;
        }

        CAltitude CInterpolatorSpline::Interpolant::interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            const double newA = evalSplineInterval(m_currentTimeMsSinceEpoc, m_pa.t[1], m_pa.t[2], m_pa.a[1], m_pa.a[2], m_pa.da[1], m_pa.da[2]);
            return CAltitude(newA, m_altitudeUnit);
        }

        void CInterpolatorSpline::Interpolant::setTimes(qint64 currentTimeMs, double timeFraction)
        {
            m_currentTimeMsSinceEpoc = currentTimeMs;
            m_pbh.setTimeFraction(timeFraction);
        }

        void CInterpolatorSpline::PosArray::initToZero()
        {
            for (int i = 0; i < 3; i++)
            {
                x[i] = 0; y[i] = 0; z[i] = 0;
                a[i] = 0; t[i] = 0;
                dx[i] = 0; dy[i] = 0; dz[i] = 0;
                da[i] = 0;
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
