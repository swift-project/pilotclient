#include "blackcore/interpolator.h"
#include "blackcore/simulator.h"
#include "blackmisc/debug.h"
#include "blackcore/plane.h"

namespace BlackCore {

	CPlane::CPlane()
		:	m_interpolator(NULL), m_driver(NULL)
	{
	}

	CPlane::CPlane(const QString &callsign, ISimulator *driver)
		:	m_callsign(callsign), m_interpolator(NULL), m_driver(driver)
	{
		m_interpolator = new CInterpolator();

		bAssert(m_interpolator);
		bAssert(driver);
	}

	void CPlane::addPosition(const CVectorGeo &position, double groundVelocity, double heading, double pitch, double bank)
	{
		bAssert(m_interpolator);

		m_interpolator->pushUpdate(position, groundVelocity, heading, pitch, bank);
	}

	void CPlane::render()
	{
		//m_driver->updatePositionAndSpeed();
	}



} // namespace BlackCore
