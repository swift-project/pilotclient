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
#include <QDateTime>

using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    void CInterpolatorLinear::initialize()
    {

    }

    void CInterpolatorLinear::addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        m_aircraftSituationList.push_back(situation);

        // Delete packets older than 30 seconds
        m_aircraftSituationList.removeBefore(QDateTime::currentDateTimeUtc().addSecs(-30));
    }

    bool CInterpolatorLinear::hasEnoughAircraftSituations() const
    {
        QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(-6);
        return !m_aircraftSituationList.findBefore(currentTime).isEmpty() && m_aircraftSituationList.size() > 1;
    }

    CAircraftSituation CInterpolatorLinear::getCurrentSituation()
    {
        QDateTime currentTime = QDateTime::currentDateTimeUtc().addSecs(-6);
        CAircraftSituationList situationsBefore = m_aircraftSituationList.findBefore(currentTime);
        CAircraftSituationList situationsAfter = m_aircraftSituationList.findAfter(currentTime);

        Q_ASSERT_X(!situationsBefore.isEmpty(), "CInterpolatorLinear::getCurrentSituation()", "List previous situations is empty!");

        CAircraftSituation beginSituation;
        CAircraftSituation endSituation;

        // The first condition covers a situation, when there is now future packet.
        // So we have to extrapolate.
        if (situationsAfter.isEmpty())
        {
            beginSituation = situationsBefore[situationsBefore.size() - 2];
            endSituation = situationsBefore[situationsBefore.size() - 1];
        }
        else
        {
            beginSituation = situationsBefore.back();
            endSituation = situationsAfter.front();
        }

        CAircraftSituation currentSituation;
        CCoordinateGeodetic currentPosition;

        // Time between start and end packet
        double deltaTime = beginSituation.msecsToAbs(endSituation);

        // Fraction of the deltaTime [0.0 - 1.0]
        double simulationTimeFraction = beginSituation.getUtcTimestamp().msecsTo(currentTime) / deltaTime;

        // Interpolate latitude: Lat = (LatB - LatA) * t + LatA
        currentPosition.setLatitude((endSituation.getPosition().latitude() - beginSituation.getPosition().latitude())
                                    * simulationTimeFraction
                                    + beginSituation.getPosition().latitude());

        // Interpolate latitude: Lon = (LonB - LonA) * t + LonA
        currentPosition.setLongitude((endSituation.getPosition().longitude() - beginSituation.getPosition().longitude())
                                     * simulationTimeFraction
                                     + beginSituation.getPosition().longitude());
        currentSituation.setPosition(currentPosition);

        // Interpolate altitude: Alt = (AltB - AltA) * t + AltA
        currentSituation.setAltitude(CAltitude((endSituation.getAltitude() - beginSituation.getAltitude())
                                               * simulationTimeFraction
                                               + beginSituation.getAltitude(),
                                               beginSituation.getAltitude().getReferenceDatum()));

        // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
        CHeading headingBegin = beginSituation.getHeading();
        CHeading headingEnd = endSituation.getHeading();

        if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
            headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());

        if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
            headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());

        currentSituation.setHeading(CHeading((headingEnd - headingBegin)
                                             * simulationTimeFraction
                                             + headingBegin,
                                             headingBegin.getReferenceNorth()));

        // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
        CAngle pitchBegin = beginSituation.getPitch();
        CAngle pitchEnd = endSituation.getPitch();

        CAngle pitch = (pitchEnd - pitchBegin) * simulationTimeFraction + pitchBegin;

        // TODO: According to the specification, pitch above horizon should be negative.
        // But somehow we get positive pitches from the network.
        pitch *= -1;
        currentSituation.setPitch(pitch);

        // Interpolate bank: Bank = (BankB - BankA) * t + BankA
        CAngle bankBegin = beginSituation.getBank();
        CAngle bankEnd = endSituation.getBank();

        CAngle bank = (bankEnd - bankBegin) * simulationTimeFraction + bankBegin;

        // TODO: According to the specification, banks to the right should be negative.
        // But somehow we get positive banks from the network.
        bank *= -1;
        currentSituation.setBank(bank);

        currentSituation.setGroundspeed((endSituation.getGroundSpeed() - beginSituation.getGroundSpeed())
                                        * simulationTimeFraction
                                        + beginSituation.getGroundSpeed());

        return currentSituation;
    }

    QDateTime CInterpolatorLinear::getTimeOfLastReceivedSituation() const
    {
        return m_aircraftSituationList.back().getUtcTimestamp();
    }
}
