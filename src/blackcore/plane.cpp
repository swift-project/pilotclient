#include "blackcore/plane.h"
#include "blackcore/interpolator.h"
#include "blackcore/simulator.h"
#include "blackmisc/debug.h"

namespace BlackCore {

	CPlane::CPlane()
		:	m_interpolator(NULL), m_driver(NULL)
	{
	}

	CPlane::CPlane(const QString &callsign, ISimulator *driver)
		:	m_callsign(callsign), m_interpolator(NULL), m_driver(driver)
	{
		m_interpolator = new CInterpolator();

		Q_ASSERT(m_interpolator);
		Q_ASSERT(driver);
	}

	void CPlane::addPosition(const CVectorGeo &position, double groundVelocity, double heading, double pitch, double bank)
	{
		Q_ASSERT(m_interpolator);

		m_interpolator->pushUpdate(position, groundVelocity, heading, pitch, bank);
	}

	void CPlane::render()
	{
		//m_driver->updatePositionAndSpeed();
	}



} // namespace BlackCore
