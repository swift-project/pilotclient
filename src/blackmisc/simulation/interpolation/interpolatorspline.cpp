// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/interpolation/interpolatorspline.h"
#include "blackmisc/simulation/interpolation/interpolatorfunctions.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc::Simulation
{
    namespace
    {
        //! \private https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
        template <size_t N>
        std::array<double, N> solveTridiagonal(std::array<std::array<double, N>, N> &matrix, std::array<double, N> &d)
        {
            // *INDENT-OFF*
            const auto a = [&matrix](size_t i) -> double & { return matrix[i][i - 1]; }; // subdiagonal
            const auto b = [&matrix](size_t i) -> double & { return matrix[i][i]; }; // main diagonal
            const auto c = [&matrix](size_t i) -> double & { return matrix[i][i + 1]; }; // superdiagonal

            // forward sweep
            c(0) /= b(0);
            d[0] /= b(0);
            for (size_t i = 1; i < N; ++i)
            {
                const double denom = b(i) - a(i) * c(i - 1);
                if (i < N - 1) { c(i) /= denom; }
                d[i] = (d[i] - a(i) * d[i - 1]) / denom;
            }

            // back substitution
            for (int i = N - 2; i >= 0; --i)
            {
                const size_t it = static_cast<size_t>(i);
                d[it] -= c(it) * d[it + 1];
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
            std::array<std::array<double, N>, N> a { {} };
            std::array<double, N> b { {} };

            // *INDENT-OFF*
            a[0][0] = 2.0 / (x[1] - x[0]);
            a[0][1] = 1.0 / (x[1] - x[0]);
            b[0] = 3.0 * (y[1] - y[0]) / ((x[1] - x[0]) * (x[1] - x[0]));

            a[N - 1][N - 2] = 1.0 / (x[N - 1] - x[N - 2]);
            a[N - 1][N - 1] = 2.0 / (x[N - 1] - x[N - 2]);
            b[N - 1] = 3.0 * (y[N - 1] - y[N - 2]) / ((x[N - 1] - x[N - 2]) * (x[N - 1] - x[N - 2]));

            for (size_t i = 1; i < N - 1; ++i)
            {
                a[i][i - 1] = 1.0 / (x[i] - x[i - 1]);
                a[i][i] = 2.0 / (x[i] - x[i - 1]) + 2.0 / (x[i + 1] - x[i]);
                a[i][i + 1] = 1.0 / (x[i + 1] - x[i]);
                b[i] = 3.0 * (y[i] - y[i - 1]) / ((x[i] - x[i - 1]) * (x[i] - x[i - 1])) + 3.0 * (y[i + 1] - y[i]) / ((x[i + 1] - x[i]) * (x[i + 1] - x[i]));
            }
            // *INDENT-ON*

            solveTridiagonal(a, b);
            return b;
        }

        //! \private Cubic interpolation.
        double evalSplineInterval(double x, double x0, double x1, double y0, double y1, double k0, double k1)
        {
            const double t = (x - x0) / (x1 - x0);
            const double a = k0 * (x1 - x0) - (y1 - y0);
            const double b = -k1 * (x1 - x0) + (y1 - y0);
            const double y = (1 - t) * y0 + t * y1 + t * (1 - t) * (a * (1 - t) + b * t);

            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(t >= 0, Q_FUNC_INFO, "Expect t >= 0");
                BLACK_VERIFY_X(t <= 1.0, Q_FUNC_INFO, "Expect t <= 1");
            }
            return y;
        }
    }

    bool CInterpolatorSpline::fillSituationsArray()
    {
        // m_s[0] .. oldest -> m_[2] .. latest
        // general idea, we interpolate from current situation -> latest situation

        // do we have the last interpolated situation?
        if (m_lastSituation.isNull())
        {
            if (m_currentSituations.isEmpty())
            {
                // nothing we can do
                m_s[0] = m_s[1] = m_s[2] = CAircraftSituation::null();
                return false;
            }
            else
            {
                // we start with the latest situation just to init the values
                CAircraftSituation f = m_currentSituations.front();
                f.setAdjustedMSecsSinceEpoch(m_currentTimeMsSinceEpoch); // adjusted time exactly "now"
                m_s[0] = m_s[1] = m_s[2] = f;
            }
        }
        else
        {
            // in normal cases init some default values
            m_s[0] = m_s[1] = m_s[2] = m_lastSituation; // current position
        }

        // set some default values
        const qint64 os = qMax(CFsdSetup::c_interimPositionTimeOffsetMsec, m_s[2].getTimeOffsetMs());
        m_s[0].addMsecs(-os); // oldest, Ref T297 default offset time to fill data
        m_s[2].addMsecs(os); // latest, Ref T297 default offset time to fill data
        if (m_currentSituations.isEmpty()) { return false; }

        // and use the real values if available
        // m_s[0] .. oldest -> m_[2] .. latest
        const CAircraftSituation latest = m_currentSituations.front();
        if (latest.isNewerThanAdjusted(m_s[1])) { m_s[2] = latest; }
        const qint64 currentAdjusted = m_s[1].getAdjustedMSecsSinceEpoch();

        // with https://dev.swift-project.org/T668#15841 avoid 2 very close positions
        // currently done by time, maybe we can also choose distance
        const qint64 osNotTooClose = qRound64(0.8 * os);
        const CAircraftSituation older = m_currentSituations.findObjectBeforeAdjustedOrDefault(currentAdjusted - osNotTooClose);
        if (!older.isNull())
        {
            m_s[0] = older;
        }
        else
        {
            const CAircraftSituation closeOlder = m_currentSituations.findObjectBeforeAdjustedOrDefault(currentAdjusted);
            if (!closeOlder.isNull()) { m_s[0] = closeOlder; }
        }
        const qint64 latestAdjusted = m_s[2].getAdjustedMSecsSinceEpoch();
        const qint64 olderAdjusted = m_s[0].getAdjustedMSecsSinceEpoch();

        // not having a new situation itself is quite normal,
        // only if it persits it is critical.
        const bool hasNewer = latestAdjusted > m_currentTimeMsSinceEpoch;

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            const bool verified = verifyInterpolationSituations(m_s[0], m_s[1], m_s[2]); // oldest -> latest, only verify order
            if (!verified)
            {
                static const QString vm("Unverified situations, m0-2 (oldest latest) %1 %2 %3");
                const QString vmValues = vm.arg(olderAdjusted).arg(currentAdjusted).arg(latestAdjusted);
                CLogMessage(this).warning(vmValues);
                Q_UNUSED(vmValues)
            }
        }
        return hasNewer;
    }

    // pin vtables to this file
    void CInterpolatorSpline::anchor()
    {}

    const IInterpolant &CInterpolatorSpline::getInterpolant(SituationLog &log)
    {
        // recalculate derivatives only if they changed
        // m_situationsLastModified updated in initIniterpolationStepData
        const bool recalculate = (m_currentTimeMsSinceEpoch >= m_nextSampleAdjustedTime) || // new step
                                 (m_situationsLastModified > m_situationsLastModifiedUsed); // modified

        if (recalculate)
        {
            // with the latest updates of T243 the order and the offsets are supposed to be correct
            // so even mixing fast/slow updates shall work
            m_situationsLastModifiedUsed = m_situationsLastModified;
            const bool fillStatus = this->fillSituationsArray();
            if (!fillStatus)
            {
                m_interpolant.setValid(false);
                return m_interpolant;
            }

            const std::array<std::array<double, 3>, 3> normals { { m_s[0].getPosition().normalVectorDouble(), m_s[1].getPosition().normalVectorDouble(), m_s[2].getPosition().normalVectorDouble() } };
            PosArray pa;
            pa.x = { { normals[0][0], normals[1][0], normals[2][0] } }; // oldest -> latest
            pa.y = { { normals[0][1], normals[1][1], normals[2][1] } };
            pa.z = { { normals[0][2], normals[1][2], normals[2][2] } }; // latest
            pa.t = { { static_cast<double>(m_s[0].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[1].getAdjustedMSecsSinceEpoch()), static_cast<double>(m_s[2].getAdjustedMSecsSinceEpoch()) } };

            pa.dx = getDerivatives(pa.t, pa.x);
            pa.dy = getDerivatives(pa.t, pa.y);
            pa.dz = getDerivatives(pa.t, pa.z);

            // - altitude unit must be the same for all three, but the unit itself does not matter
            // - ground elevantion here normally is not available
            // - some info how fast a plane moves: 100km/h => 1sec 27,7m => 5 secs 136m
            // - on an airport the plane does not move very fast, or not at all
            // - and the elevation remains (almost) constant for a wider area
            // - during flying the ground elevation not really matters
            this->updateElevations(true);
            static const CLengthUnit altUnit = CAltitude::defaultUnit();
            const CLength cg(this->getModelCG().switchedUnit(altUnit));
            const double a0 = m_s[0].getCorrectedAltitude(cg).value(altUnit); // oldest
            const double a1 = m_s[1].getCorrectedAltitude(cg).value(altUnit);
            const double a2 = m_s[2].getCorrectedAltitude(cg).value(altUnit); // latest
            pa.a = { { a0, a1, a2 } };
            pa.gnd = { { m_s[0].getOnGroundInfo().getGroundFactor(), m_s[1].getOnGroundInfo().getGroundFactor(), m_s[2].getOnGroundInfo().getGroundFactor() } };
            pa.da = getDerivatives(pa.t, pa.a);
            pa.dgnd = getDerivatives(pa.t, pa.gnd);

            m_prevSampleAdjustedTime = m_s[1].getAdjustedMSecsSinceEpoch();
            m_nextSampleAdjustedTime = m_s[2].getAdjustedMSecsSinceEpoch(); // latest
            m_prevSampleTime = m_s[1].getMSecsSinceEpoch(); // last interpolated situation normally
            m_nextSampleTime = m_s[2].getMSecsSinceEpoch(); // latest
            m_interpolant = CInterpolant(pa, altUnit, CInterpolatorLinearPbh(m_s[1], m_s[2])); // older, newer
            Q_ASSERT_X(m_prevSampleAdjustedTime < m_nextSampleAdjustedTime, Q_FUNC_INFO, "Wrong time order");
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
        // KB: is that correct with dt2, or would it be m_nextSampleTime - m_prevSampleTime
        //     as long as the offset time is constant, it does not matter
        const double dt1 = static_cast<double>(m_currentTimeMsSinceEpoch - m_prevSampleAdjustedTime);
        const double dt2 = static_cast<double>(m_nextSampleAdjustedTime - m_prevSampleAdjustedTime);
        double timeFraction = dt1 / dt2;

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(dt1 >= 0, Q_FUNC_INFO, "Expect postive dt1");
            BLACK_VERIFY_X(dt2 > 0, Q_FUNC_INFO, "Expect postive dt2");
            BLACK_VERIFY_X(isAcceptableTimeFraction(timeFraction), Q_FUNC_INFO, "Expect fraction 0-1");
        }
        timeFraction = clampValidTimeFraction(timeFraction);
        const qint64 interpolatedTime = m_prevSampleTime + qRound64(timeFraction * dt2);

        // time fraction is expected between 0-1
        m_currentInterpolationStatus.setInterpolated(true);
        m_interpolant.setTimes(m_currentTimeMsSinceEpoch, timeFraction, interpolatedTime);
        m_interpolant.setRecalculated(recalculate);

        if (this->doLogging())
        {
            log.interpolationSituations.clear();
            log.interpolationSituations.push_back(m_s[0]);
            log.interpolationSituations.push_back(m_s[1]);
            log.interpolationSituations.push_back(m_s[2]); // latest at end
            log.interpolator = 's';
            log.deltaSampleTimesMs = dt2;
            log.simTimeFraction = timeFraction;
            log.tsInterpolated = interpolatedTime; // without offsets
            log.interpolantRecalc = m_interpolant.isRecalculated();
        }

        return m_interpolant;
    }

    bool CInterpolatorSpline::updateElevations(bool canSkip)
    {
        bool updated = false;
        for (CAircraftSituation &s : m_s)
        {
            if (s.hasGroundElevation()) { continue; } // do not override existing values
            if (canSkip && s.canLikelySkipNearGroundInterpolation()) { continue; }

            const CElevationPlane plane = this->findClosestElevationWithinRange(s, CElevationPlane::singlePointRadius());
            const bool u = s.setGroundElevationChecked(plane, CAircraftSituation::FromCache);
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

    CInterpolatorSpline::CInterpolant::CInterpolant(const CInterpolatorSpline::PosArray &pa, const CLengthUnit &altitudeUnit, const CInterpolatorLinearPbh &pbh) : m_pa(pa), m_altitudeUnit(altitudeUnit)
    {
        m_pbh = pbh;
    }

    std::tuple<Geo::CCoordinateGeodetic, Aviation::CAltitude> CInterpolatorSpline::CInterpolant::interpolatePositionAndAltitude() const
    {
        const double t1 = m_pa.t[1];
        const double t2 = m_pa.t[2]; // latest (adjusted)

        bool valid = (t1 < t2) && (m_currentTimeMsSinceEpoc >= t1) && (m_currentTimeMsSinceEpoc < t2);
        if (!valid && CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(t1 < t2, Q_FUNC_INFO, "Expect sorted times, latest first"); // that means a bug in our code init the values
            BLACK_VERIFY_X(m_currentTimeMsSinceEpoc >= t1, Q_FUNC_INFO, "invalid timestamp t1");
            BLACK_VERIFY_X(m_currentTimeMsSinceEpoc < t2, Q_FUNC_INFO, "invalid timestamp t2"); // t1==t2 results in div/0
        }
        if (!valid) { return { {}, {} }; }

        const double newX = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.x[1], m_pa.x[2], m_pa.dx[1], m_pa.dx[2]);
        const double newY = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.y[1], m_pa.y[2], m_pa.dy[1], m_pa.dy[2]);
        const double newZ = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.z[1], m_pa.z[2], m_pa.dz[1], m_pa.dz[2]);

        valid = CAircraftSituation::isValidVector(m_pa.x) && CAircraftSituation::isValidVector(m_pa.y) && CAircraftSituation::isValidVector(m_pa.z);
        if (!valid && CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(CAircraftSituation::isValidVector(m_pa.x), Q_FUNC_INFO, "invalid X"); // all x values
            BLACK_VERIFY_X(CAircraftSituation::isValidVector(m_pa.y), Q_FUNC_INFO, "invalid Y"); // all y values
            BLACK_VERIFY_X(CAircraftSituation::isValidVector(m_pa.z), Q_FUNC_INFO, "invalid Z"); // all z values
        }
        if (!valid) { return { {}, {} }; }

        const std::array<double, 3> normalVector = { { newX, newY, newZ } };
        const CCoordinateGeodetic currentPosition(normalVector);

        valid = CAircraftSituation::isValidVector(normalVector);
        if (!valid && CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(valid, Q_FUNC_INFO, "invalid vector");
            CLogMessage(this).warning(u"Invalid vector v: %2 %3 %4") << normalVector[0] << normalVector[1] << normalVector[2];
        }
        if (!valid) { return { {}, {} }; }

        const double newA = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, m_pa.a[1], m_pa.a[2], m_pa.da[1], m_pa.da[2]);
        const CAltitude alt(newA, m_altitudeUnit);

        return { currentPosition, alt };
    }

    Aviation::COnGroundInfo CInterpolatorSpline::CInterpolant::interpolateGroundFactor() const
    {
        const double t1 = m_pa.t[1];
        const double t2 = m_pa.t[2]; // latest (adjusted)
        bool valid = (t1 < t2) && (m_currentTimeMsSinceEpoc >= t1) && (m_currentTimeMsSinceEpoc < t2);
        if (!valid) { return { COnGroundInfo::OnGroundSituationUnknown, COnGroundInfo::NotSetGroundDetails }; }

        const double gnd1 = m_pa.gnd[1];
        const double gnd2 = m_pa.gnd[2]; // latest

        if (CAircraftSituation::isGfEqualAirborne(gnd1, gnd2))
        {
            return { COnGroundInfo::NotOnGround, COnGroundInfo::OnGroundByInterpolation };
        }
        else if (CAircraftSituation::isGfEqualOnGround(gnd1, gnd2))
        {
            return { COnGroundInfo::OnGround, COnGroundInfo::OnGroundByInterpolation };
        }
        else
        {
            const double newGnd = evalSplineInterval(m_currentTimeMsSinceEpoc, t1, t2, gnd1, gnd2, m_pa.dgnd[1], m_pa.dgnd[2]);
            return COnGroundInfo(newGnd);
        }
    }

    void CInterpolatorSpline::CInterpolant::setTimes(qint64 currentTimeMs, double timeFraction, qint64 interpolatedTimeMs)
    {
        m_currentTimeMsSinceEpoc = currentTimeMs;
        m_interpolatedTime = interpolatedTimeMs;
        m_pbh.setTimeFraction(timeFraction);
    }

    void CInterpolatorSpline::PosArray::initToZero()
    {
        for (uint i = 0; i < 3; i++)
        {
            x[i] = 0;
            y[i] = 0;
            z[i] = 0;
            a[i] = 0;
            t[i] = 0;
            dx[i] = 0;
            dy[i] = 0;
            dz[i] = 0;
            da[i] = 0;
            gnd[i] = 0;
            dgnd[i] = 0;
        }
    }

    const CInterpolatorSpline::PosArray &CInterpolatorSpline::PosArray::zeroPosArray()
    {
        static const PosArray pa = [] {
            PosArray p;
            p.initToZero();
            return p;
        }();
        return pa;
    }

    bool CInterpolatorSpline::verifyInterpolationSituations(const CAircraftSituation &oldest, const CAircraftSituation &newer, const CAircraftSituation &latest, const CInterpolationAndRenderingSetupPerCallsign &setup)
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
        if (situations.containsOnGroundDetails(COnGroundInfo::InFromParts))
        {
            // if a client supports parts, all ground situations are supposed to be parts based
            details = situations.areAllOnGroundDetailsSame(COnGroundInfo::InFromParts);
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

} // ns
