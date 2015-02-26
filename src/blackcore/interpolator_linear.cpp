/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolator_linear.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/logmessage.h"
#include <QDateTime>

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    CAircraftSituation CInterpolatorLinear::getInterpolatedSituation(const CCallsign &callsign, qint64 currentTimeMsSinceEpoc, InterpolationStatus &status, const CSituationsPerCallsign *situationsPerCallsign) const
    {
        const static CAircraftSituation empty;
        status.reset();
        QList<CAircraftSituationList> splitSituations;
        if (currentTimeMsSinceEpoc < 0) { currentTimeMsSinceEpoc = QDateTime::currentMSecsSinceEpoch(); }
        qint64 splitTimeMsSinceEpoch = currentTimeMsSinceEpoc - TimeOffsetMs;

        if (situationsPerCallsign)
        {
            if (!situationsPerCallsign->contains(callsign)) { return empty; }
            if ((*situationsPerCallsign)[callsign].isEmpty())  { return empty; }
            splitSituations = (*situationsPerCallsign)[callsign].splitByTime(splitTimeMsSinceEpoch);
        }
        else
        {
            // only part where it is locked
            QReadLocker lock(&m_lockSituations);
            if (!m_situationsByCallsign.contains(callsign)) { return empty; }
            if (m_situationsByCallsign[callsign].isEmpty())  { return empty; }
            splitSituations = m_situationsByCallsign[callsign].splitByTime(splitTimeMsSinceEpoch);
        }

        CAircraftSituationList &situationsNewer = splitSituations[0]; // newer part
        CAircraftSituationList &situationsOlder = splitSituations[1]; // older part

        // interpolation situations
        CAircraftSituation oldSituation;
        CAircraftSituation newSituation;
        int situationsNewerNo = situationsNewer.size();
        int situationsOlderNo = situationsOlder.size();

        // latest first, now 00:26 -> 00:26 - 6000ms -> 00:20 split time
        // time     pos
        // 00:25    10    newer
        // 00:20    11    newer
        // <----- split
        // 00:15    12    older
        // 00:10    13    older
        // 00:05    14    older

        // The first condition covers a situation, when there are no before / after situations.
        // We just place at he last position until we get before / after situations
        if (situationsOlderNo < 1 || situationsNewerNo < 1)
        {
            // no after situations
            if (situationsOlderNo < 1) { return situationsNewer.back(); }  // oldest newest

            // no before situations
            if (situationsOlder.size() < 2) { return situationsOlder.front(); } // latest older

            // this will lead to extrapolation
            oldSituation = situationsOlder[1];       // before newest
            newSituation = situationsOlder.front();  // newest

        }
        else
        {
            oldSituation = situationsOlder.front(); // first oldest
            newSituation = situationsNewer.back();  // latest newest
            Q_ASSERT(oldSituation.getMSecsSinceEpoch() < newSituation.getMSecsSinceEpoch());
        }

        CAircraftSituation currentSituation(oldSituation);
        CCoordinateGeodetic currentPosition;
        status.interpolationSucceeded = true;

        // Time between start and end packet
        double deltaTime = oldSituation.absMsecsTo(newSituation);

        // Fraction of the deltaTime, ideally [0.0 - 1.0]
        // < 0 should not happen due to the split, > 1 can happen if new values are delayed beyond split time
        // 1) values > 1 mean extrapolation
        // 2) values > 2 mean no new situations coming in
        double simulationTimeFraction = 1 - ((newSituation.getMSecsSinceEpoch() - splitTimeMsSinceEpoch) / deltaTime);
        if (simulationTimeFraction > 2.0)
        {
            if (this->m_withDebugMsg)
            {
                CLogMessage(this).warning("Extrapolation, fraction > 1: %1 for callsign: %2") << simulationTimeFraction << callsign;
            }
        }

        // Interpolate latitude: Lat = (LatB - LatA) * t + LatA
        const CLatitude oldLat(oldSituation.latitude());
        const CLatitude newLat(newSituation.latitude());
        const CLongitude oldLng(oldSituation.longitude());
        const CLongitude newLng(newSituation.longitude());

        currentPosition.setLatitude((newLat - oldLat)
                                    * simulationTimeFraction
                                    + oldLat);

        // Interpolate latitude: Lon = (LonB - LonA) * t + LonA
        currentPosition.setLongitude((newLng - oldLng)
                                     * simulationTimeFraction
                                     + oldLng);
        currentSituation.setPosition(currentPosition);

        // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
        const CAltitude oldAlt(oldSituation.getAltitude());
        const CAltitude newAlt(newSituation.getAltitude());
        Q_ASSERT(oldAlt.getReferenceDatum() == newAlt.getReferenceDatum()); // otherwise no calculation is possible
        currentSituation.setAltitude(CAltitude((newAlt - oldAlt)
                                               * simulationTimeFraction
                                               + oldAlt,
                                               oldAlt.getReferenceDatum()));

        if (newLat == oldLat && newLng == oldLng && oldAlt == newAlt)
        {
            return currentSituation;
        }

        // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
        CHeading headingBegin = oldSituation.getHeading();
        CHeading headingEnd = newSituation.getHeading();

        if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
        {
            headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
        }

        if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
        {
            headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
        }

        currentSituation.setHeading(CHeading((headingEnd - headingBegin)
                                             * simulationTimeFraction
                                             + headingBegin,
                                             headingBegin.getReferenceNorth()));

        // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
        CAngle pitchBegin = oldSituation.getPitch();
        CAngle pitchEnd = newSituation.getPitch();
        CAngle pitch = (pitchEnd - pitchBegin) * simulationTimeFraction + pitchBegin;

        // TODO: According to the specification, pitch above horizon should be negative.
        // But somehow we get positive pitches from the network.
        pitch *= -1;
        currentSituation.setPitch(pitch);

        // Interpolate bank: Bank = (BankB - BankA) * t + BankA
        CAngle bankBegin = oldSituation.getBank();
        CAngle bankEnd = newSituation.getBank();
        CAngle bank = (bankEnd - bankBegin) * simulationTimeFraction + bankBegin;

        // TODO: According to the specification, banks to the right should be negative.
        // But somehow we get positive banks from the network.
        bank *= -1.0;
        currentSituation.setBank(bank);

        currentSituation.setGroundspeed((newSituation.getGroundSpeed() - oldSituation.getGroundSpeed())
                                        * simulationTimeFraction
                                        + oldSituation.getGroundSpeed());
        status.changedPosition = true;
        Q_ASSERT(currentSituation.getCallsign() == callsign);
        return currentSituation;
    }

} // namespace
