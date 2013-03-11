#include <iostream>
#include "blackcore/matrix_3d.h"
#include "blackcore/vector_geo.h"
#include "blackcore/math.h"
#include "blackcore/interpolator.h"
#include "blackcore/constants.h"

namespace BlackCore
{

CInterpolator::CInterpolator()
    :   m_state_begin(NULL), m_state_end(NULL)
{
    m_time.start();
}

CInterpolator::~CInterpolator()
{
	delete m_state_begin;
	delete m_state_end;
}

void CInterpolator::initialize()
{

}

void CInterpolator::pushUpdate(CVectorGeo pos, double groundVelocity, double heading, double pitch, double bank)
{
    CNed vNED;

    if ( m_state_begin == NULL )
    {
         m_state_begin = new TPlaneState();

         m_state_begin->position = pos.toCartesian();
		 m_state_begin->orientation.heading = heading*Constants::DegToRad;
         m_state_begin->orientation.pitch = pitch*Constants::DegToRad;
         m_state_begin->orientation.bank = bank*Constants::DegToRad;
		 m_state_begin->groundspeed = groundVelocity * Constants::KnotsToMeterPerSecond;

         vNED.setNorth( cos(m_state_begin->orientation.heading)*m_state_begin->groundspeed );
         vNED.setEast( sin(m_state_begin->orientation.heading)*m_state_begin->groundspeed );
         vNED.setDown(0);
         vNED.setPosition(pos);

         m_state_begin->velocity = vNED.toECEF();


         m_state_begin->timestamp = 0;
         return;
    }
    else
    {
        stateNow(m_state_begin);
    }

    if ( m_state_end == NULL )
    {
         m_state_end   = new TPlaneState();
    }
    m_state_end->reset();
	
    m_state_end->timestamp = m_time.elapsed();

    m_state_end->position = pos.toCartesian();
    m_state_end->orientation.heading = normalizeRadians(heading*Constants::DegToRad);
    m_state_end->orientation.pitch = normalizeRadians(pitch*Constants::DegToRad);
    m_state_end->orientation.bank = normalizeRadians(bank*Constants::DegToRad);
    m_state_end->groundspeed = groundVelocity*Constants::KnotsToMeterPerSecond;

    vNED.setNorth( cos(m_state_end->orientation.heading)*m_state_end->groundspeed );
    vNED.setEast( sin(m_state_end->orientation.heading)*m_state_end->groundspeed );
    vNED.setDown(0);
    vNED.setPosition(pos);
    m_state_end->velocity = vNED.toECEF();

	std::cout << " Interpolator End velocity: "    << std::endl;
	vNED.print();
    std::cout << std::endl;

    m_timeEnd = 5;

    double m_TFpow4     = CMath::cubic(m_timeEnd) * m_timeEnd;

    m_a = m_state_begin->velocity * CMath::square(m_timeEnd);
    m_a += m_state_end->velocity * CMath::square(m_timeEnd);

    m_a += m_state_begin->position * m_timeEnd * 2;
    m_a -= m_state_end->position * m_timeEnd * 2;
    m_a *= 6;
    m_a /= m_TFpow4;

    m_b = m_state_begin->velocity * CMath::cubic(m_timeEnd) * (-2) - m_state_end->velocity * CMath::cubic(m_timeEnd);
    m_b = m_b - m_state_begin->position * CMath::square(m_timeEnd) * 3 + m_state_end->position * CMath::square(m_timeEnd) * 3;
    m_b = m_b * 2 / ( m_TFpow4 );
}

bool CInterpolator::isValid()
{
    return (m_state_begin && m_state_end);
}

bool CInterpolator::stateNow(TPlaneState *state)
{
    if ( !isValid() )
        return false;

    double time = 5;
	
	/*!
	  Plane Position
	*/

    double timePow2 = CMath::square(time);
    double timePow3 = CMath::cubic(time);

    CEcef pos;
    pos = m_b*3*timePow2*m_timeEnd + m_a * timePow3 * m_timeEnd - m_b * 3 * time * CMath::square(m_timeEnd) - m_a * time* CMath::cubic(m_timeEnd);
    pos += m_state_begin->position*(-6)*time + m_state_begin->position*6*m_timeEnd + m_state_end->position*6*time;
    pos /= 6*m_timeEnd;

    state->position = pos;
	
	CEcef vel;
	vel.zeros();
	vel = m_a * ( 3 * m_timeEnd * CMath::square(time) - CMath::cubic(m_timeEnd));
	vel += m_b * ( 6 * m_timeEnd * time - 3 * CMath::square(m_timeEnd)) + (m_state_end->position - m_state_begin->position) * 6;
	vel /= 6*m_timeEnd;
	
	state->velocity = vel;
	state->velNED = vel.toNED(pos.toGeodetic());
	
	/*!
	  Plane Orientation
	*/

	double vEast = state->velNED.East();
	double vNorth = state->velNED.North();
	double fraction = vNorth / vEast;
	
	double heading = atan2 (vNorth, vEast);

	state->orientation.heading = heading * Constants::RadToDeg;

	return true;

}

double CInterpolator::normalizeRadians(double radian)
{
	return radian - Constants::TwoPI * floor(0.5 + radian / Constants::TwoPI);
}

} // namespace BlackCore
