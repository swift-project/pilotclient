#include <math.h>
#include <iostream>

#include "blackcore/constants.h"
#include "blackcore/vector_geo.h"
#include "blackcore/ecef.h"

namespace BlackCore
{

    CVectorGeo::CVectorGeo()
    {
        zeros();
    }

    CVectorGeo::CVectorGeo(double lat, double lon, double alt)
    {
        zeros();
        m_latitude = lat;
        m_longitude = lon;
        m_altitude = alt;
    }

    CVectorGeo::CVectorGeo(const CVectorGeo &other)
    {
        zeros();
        m_latitude = other.latitude();
        m_longitude = other.longitude();
        m_altitude = other.altitude();
    }

    CEcef CVectorGeo::toCartesian()
    {
        CEcef result;
		
		double phi = m_latitude * Constants::DegToRad;
        double lambda = m_longitude * Constants::DegToRad;
		double sphi = sin(phi);
		double cphi = 0;
		if (std::abs(m_latitude) != 90)
			cphi = cos(phi);
			
		double n = Constants::EarthRadius/sqrt(1-Constants::e2 * CMath::square(sphi));
		
		double slambda = 0;
		if (m_longitude != -180)
			slambda = sin(lambda);
		
		double clambda = 0;
		if (std::abs(m_longitude) != 90)
			clambda = cos(lambda);
        
		double h = m_altitude * Constants::FeetToMeter;
		
		double X = (n + h) * cphi;
		double Y = X * slambda;
		X *= clambda;
		double Z = (Constants::e2m * n + h)*sphi;

        result.setX(X);
        result.setY(Y);
        result.setZ(Z);
        return result;
    }

    void CVectorGeo::zeros()
    {
        m_latitude = 0;
        m_longitude = 0;
        m_altitude = 0;
    }
	
	void CVectorGeo::print()
{
    std::cout << "v = " << std::endl;
	std::cout << std::fixed;
    std::cout << "[" << m_latitude << "]" << std::endl;
	std::cout << "[" << m_longitude << "]" << std::endl;
	std::cout << "[" << m_altitude << "]" << std::endl;
}

    CVectorGeo &CVectorGeo::operator =(const CVectorGeo &rhs)
    {
        if (this != &rhs)
        {
            m_latitude = rhs.latitude();
            m_longitude = rhs.longitude();
            m_altitude = rhs.altitude();
        }
        return *this;
    }

} // namespace BlackCore
