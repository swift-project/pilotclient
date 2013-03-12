#include <blackcore/simulator.h>
#include <blackcore/vector_geo.h>
#include "blackcore/plane.h"
#include "blackcore/multiplayer.h"

namespace BlackCore {

CMultiPlayer::CMultiPlayer()
    :   m_isRunning(false)
{
    registerMessageFunction(this, &CMultiPlayer::onPositionUpdate);

    m_simulator = ISimulator::createDriver(ISimulator::FSX);
}

void CMultiPlayer::start()
{
    if (m_isRunning)
        return;

    m_isRunning = true;
}

void CMultiPlayer::stop()
{
    if (!m_isRunning)
        return;

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

	CVectorGeo position(plane_position->Latitude(), plane_position->Longitude(), plane_position->Altitude());

	FS_PBH pitchBankHeading;
	pitchBankHeading.pbh = plane_position->PBH();

	//! TODO: Pitch Bank Heading and a timestamp

    if (plane)
    {
		plane->addPosition(position, plane_position->Speed(), pitchBankHeading.hdg, pitchBankHeading.pitch, pitchBankHeading.bank);
    }
    else
    {
        plane = new CPlane(callsign, m_simulator);
        addPlane(plane);
		plane->addPosition(position, plane_position->Speed(), pitchBankHeading.hdg, pitchBankHeading.pitch, pitchBankHeading.bank);
    }

}

void CMultiPlayer::addPlane(CPlane *plane)
{
    m_multiplayer_planes.insert(plane->Callsign(), plane);
}

void CMultiPlayer::removePlane(CPlane *plane)
{
	qint32 id;
	m_simulator->removePlane(id);
}

} //! namespace BlackCore
