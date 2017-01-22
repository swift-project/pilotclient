/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolator.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include <QDateTime>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        IInterpolator::IInterpolator(IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent) :
            QObject(parent),
            CRemoteAircraftAware(provider)
        {
            Q_ASSERT_X(provider, Q_FUNC_INFO, "missing provider");
            this->setObjectName(objectName);
        }

        BlackMisc::Aviation::CAircraftSituation IInterpolator::getInterpolatedSituation(
            const CCallsign &callsign, qint64 currentTimeSinceEpoc,
            const CInterpolationHints &hints, InterpolationStatus &status) const
        {
            // has to be thread safe

            status.reset();
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");

            auto currentSituation = this->getInterpolatedSituation(this->remoteAircraftSituations(callsign), currentTimeSinceEpoc, hints, status);
            currentSituation.setCallsign(callsign); // make sure callsign is correct
            return currentSituation;
        }

        CAircraftParts IInterpolator::getInterpolatedParts(const CAircraftPartsList &parts, qint64 currentTimeMsSinceEpoch, IInterpolator::PartsStatus &partsStatus) const
        {
            partsStatus.reset();

            if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }

            // find the first parts not in the correct order, keep only the parts before that one
            const auto end = std::is_sorted_until(parts.begin(), parts.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            const auto validParts = makeRange(parts.begin(), end);

            // stop if we don't have any parts
            if (validParts.isEmpty()) { return {}; }
            partsStatus.setSupportsParts(true);

            // find the first parts earlier than the current time
            const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto && p) { return p.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoch; });
            const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
            const auto partsOlder = makeRange(pivot, validParts.end());

            if (partsOlder.isEmpty()) { return *(partsNewer.end() - 1); }
            CAircraftParts currentParts = partsOlder.front();
            if (currentParts.isOnGround()) { return currentParts; }

            // here we know aircraft is not on ground, and we check if it was recently on ground or if it will be on ground soon
            const auto latestTakeoff = std::adjacent_find(partsOlder.begin(), partsOlder.end(), [](auto &&, auto && p) { return p.isOnGround(); });
            const auto soonestLanding = std::find_if(partsNewer.begin(), partsNewer.end(), [](auto && p) { return p.isOnGround(); });

            // our clairvoyance is limited by the time offset
            const double significantPast = 5.0; // \fixme 20170121 KB would it make sense to centrally define the update time (5secs), in case it changes. I see a lot of 5.0 hardcoded here
            const double predictableFuture = soonestLanding == partsNewer.end() ? 5.0 : std::min(5.0, static_cast<double>(soonestLanding->getTimeOffsetMs()) / 1000.0);

            const double secondsSinceTakeoff = latestTakeoff == partsOlder.end() ? 5.0 : (currentTimeMsSinceEpoch - latestTakeoff->getAdjustedMSecsSinceEpoch()) / 1000.0;
            const double secondsUntilLanding = soonestLanding == partsNewer.end() ? 5.0 : (soonestLanding->getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoch) / 1000.0;
            Q_ASSERT(secondsSinceTakeoff >= 0.0);
            Q_ASSERT(secondsUntilLanding >= 0.0);

            const double takeoffFactor = secondsSinceTakeoff / significantPast;
            const double landingFactor = secondsUntilLanding / predictableFuture;
            const double airborneFactor = std::min(std::min(takeoffFactor, landingFactor), 1.0);
            currentParts.setOnGroundInterpolated(1.0 - smootherStep(airborneFactor));
            return currentParts;
        }

        CAircraftParts IInterpolator::getInterpolatedParts(const CCallsign &callsign, qint64 currentTimeMsSinceEpoch, IInterpolator::PartsStatus &partsStatus) const
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            partsStatus.reset();

            partsStatus.setSupportsParts(this->isRemoteAircraftSupportingParts(callsign));
            if (!partsStatus.isSupportingParts()) { return {}; }
            return this->getInterpolatedParts(this->remoteAircraftParts(callsign, -1), currentTimeMsSinceEpoch, partsStatus);
        }

        void IInterpolator::setInterpolatorSetup(const CInterpolationAndRenderingSetup &setup)
        {
            QWriteLocker l(&m_lock);
            m_setup = setup;
        }

        CInterpolationAndRenderingSetup IInterpolator::getInterpolatorSetup() const
        {
            QReadLocker l(&m_lock);
            return m_setup;
        }

        void IInterpolator::setGroundElevationFromHint(const CInterpolationHints &hints, CAircraftSituation &situation)
        {
            if (situation.hasGroundElevation()) { return; }
            const CAltitude elevation = hints.getGroundElevation(situation);
            if (elevation.isNull()) { return; }
            situation.setGroundElevation(elevation);
        }

        void IInterpolator::setGroundFlagFromInterpolator(const CInterpolationHints &hints, double groundFactor, CAircraftSituation &situation)
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

            // no value by factor, guess on elevation
            const CLength heightAboveGround(situation.getHeightAboveGround());
            const CLength cgAboveGround(hints.getCGAboveGround());
            if (!heightAboveGround.isNull())
            {
                const bool og = cgAboveGround.isNull() ?
                                heightAboveGround.value(CLengthUnit::m()) < 1.0 :
                                heightAboveGround <= cgAboveGround;
                situation.setOnGround(og ? CAircraftSituation::OnGround : CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByElevation);
            }

            // for VTOL aircraft we give up
            if (hints.isVtolAircraft())
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

        bool IInterpolator::InterpolationStatus::allTrue() const
        {
            return m_interpolationSucceeded && m_changedPosition;
        }

        void IInterpolator::InterpolationStatus::reset()
        {
            m_changedPosition = false;
            m_interpolationSucceeded = false;
        }

        bool IInterpolator::PartsStatus::allTrue() const
        {
            return m_supportsParts;
        }

        void IInterpolator::PartsStatus::reset()
        {
            m_supportsParts = false;
        }
    } // namespace
} // namespace
