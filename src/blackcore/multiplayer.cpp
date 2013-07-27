/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/simulator.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackcore/plane.h"
#include "blackcore/multiplayer.h"
#include "blackmisc/avheading.h"

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackCore
{

CMultiPlayer::CMultiPlayer() : m_isRunning(false)
{
    registerMessageFunction(this, &CMultiPlayer::onPositionUpdate);
    m_simulator = ISimulator::createDriver(ISimulator::FSX);
}

void CMultiPlayer::start()
{
    if (m_isRunning) return;
    m_isRunning = true;
}

void CMultiPlayer::stop()
{
    if (!m_isRunning) return;
    m_isRunning = false;
}

void CMultiPlayer::run()
{
    TPlaneManager::iterator it;
    for (it = m_multiplayer_planes.begin(); it != m_multiplayer_planes.end(); ++it)
    {
        if (needsToRemoved(it.value()))
        {
            removePlane(it.value());
            it = m_multiplayer_planes.erase(it);
        }

        if (areAIPlanesEnabled())
            it.value()->render();
    }
}

bool CMultiPlayer::isKnown(const QString &callsign) const
{
    return m_multiplayer_planes.contains(callsign);
}

CPlane *CMultiPlayer::getPlane(const QString &callsign)
{
    return m_multiplayer_planes.value(callsign);
}

void CMultiPlayer::onPositionUpdate(const FSD::FSD_MSG_Plane_Position *plane_position)
{
    QString callsign = plane_position->Callsign();
    CPlane *plane;

    plane = getPlane(callsign);
    if (!plane)
    {
        plane = new CPlane(callsign, m_simulator);
        addPlane(plane);
    }


    CCoordinateGeodetic position(plane_position->Latitude(), plane_position->Longitude(), plane_position->Altitude());
    FS_PBH pitchBankHeading;
    pitchBankHeading.pbh = plane_position->PBH();

    // TODO: Pitch Bank Heading and a timestamp
    // TODO: Usage of physical quantities with FSD::FSD_MSG_Plane_Position

    plane->addPosition(position,
                       CSpeed(plane_position->Speed(), CSpeedUnit::kts()),
                       CHeading((qint32)pitchBankHeading.hdg, false, CAngleUnit::deg()),
                       CAngle((qint32)pitchBankHeading.pitch, CAngleUnit::deg()),
                       CAngle((qint32)pitchBankHeading.bank, CAngleUnit::deg()));

}

void CMultiPlayer::addPlane(CPlane *plane)
{
    m_multiplayer_planes.insert(plane->callsign(), plane);
}

void CMultiPlayer::removePlane(CPlane * /** plane **/)
{
    qint32 id = 0;
    m_simulator->removePlane(id);
}

} // namespace
