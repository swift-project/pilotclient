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
            }

            //! \private Linear equation expressed as tridiagonal matrix.
            //! https://en.wikipedia.org/wiki/Spline_interpolation
            //! http://blog.ivank.net/interpolation-with-cubic-splines.html
            template <size_t N>
            std::array<double, N> getDerivatives(const std::array<double, N> &x, const std::array<double, N> &y)
            {
                std::array<std::array<double, N>, N> a {{}};
                std::array<double, N> b {{}};

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
            const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status, CInterpolationLogger::SituationLog &log)
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

                if (situationsNewer.isEmpty() || situationsOlder.size() < 2) { return { *this, 0 }; }

                const std::array<CAircraftSituation, 3> s {{ *(situationsOlder.begin() + 1), *situationsOlder.begin(), *(situationsNewer.end() - 1) }};

                const std::array<std::array<double, 3>, 3> normals {{ s[0].getPosition().normalVectorDouble(), s[1].getPosition().normalVectorDouble(), s[2].getPosition().normalVectorDouble() }};
                x = {{ normals[0][0], normals[1][0], normals[2][0] }};
                y = {{ normals[0][1], normals[1][1], normals[2][1] }};
                z = {{ normals[0][2], normals[1][2], normals[2][2] }};
                a = {{ s[0].getCorrectedAltitude().value(), s[1].getCorrectedAltitude().value(), s[2].getCorrectedAltitude().value() }};
                t = {{ static_cast<double>(s[0].getAdjustedMSecsSinceEpoch()), static_cast<double>(s[1].getAdjustedMSecsSinceEpoch()), static_cast<double>(s[2].getAdjustedMSecsSinceEpoch()) }};

                dx = getDerivatives(t, x);
                dy = getDerivatives(t, y);
                dz = getDerivatives(t, z);
                da = getDerivatives(t, a);

                m_prevSampleTime = situationsOlder.begin()->getAdjustedMSecsSinceEpoch();
                m_nextSampleTime = (situationsNewer.end() - 1)->getAdjustedMSecsSinceEpoch();
                m_altitudeUnit = situationsOlder.begin()->getAltitude().getUnit();
                m_pbh = { *situationsOlder.begin(), *(situationsNewer.end() - 1) };
            }
            log.interpolator = 's';
            log.oldSituation = m_pbh.getOldSituation();
            log.newSituation = m_pbh.getNewSituation();

            status.setInterpolationSucceeded(true);
            status.setChangedPosition(true);
            const double dt1 = static_cast<double>(currentTimeMsSinceEpoc - m_prevSampleTime);
            const double dt2 = static_cast<double>(m_nextSampleTime - m_prevSampleTime);
            const double timeFraction = dt1 / dt2;
            log.deltaTimeMs = dt1;
            log.deltaTimeFractionMs = dt2;
            log.simulationTimeFraction = timeFraction;
            m_pbh.setTimeFraction(timeFraction);

            return { *this, currentTimeMsSinceEpoc };
        }

        CCoordinateGeodetic CInterpolatorSpline::Interpolant::interpolatePosition(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            const double newX = evalSplineInterval(currentTimeMsSinceEpoc, i.t[1], i.t[2], i.x[1], i.x[2], i.dx[1], i.dx[2]);
            const double newY = evalSplineInterval(currentTimeMsSinceEpoc, i.t[1], i.t[2], i.y[1], i.y[2], i.dy[1], i.dy[2]);
            const double newZ = evalSplineInterval(currentTimeMsSinceEpoc, i.t[1], i.t[2], i.z[1], i.z[2], i.dz[1], i.dz[2]);

            CCoordinateGeodetic currentPosition;
            currentPosition.setNormalVector(newX, newY, newZ);
            return currentPosition;
        }

        CAltitude CInterpolatorSpline::Interpolant::interpolateAltitude(const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints) const
        {
            Q_UNUSED(setup);
            Q_UNUSED(hints);

            const double newA = evalSplineInterval(currentTimeMsSinceEpoc, i.t[1], i.t[2], i.a[1], i.a[2], i.da[1], i.da[2]);

            return CAltitude(newA, i.m_altitudeUnit);
        }
    }
}
