#include <math.h>
#include <iostream>

#include "blackcore/constants.h"
#include "blackcore/vector_geo.h"
#include "blackcore/ecef.h"

namespace BlackCore
{

    CVectorGeo::CVectorGeo()
        : m_latitudeDegrees(0), m_longitudeDegrees(0), m_altitudeMeters(0)
    {}

    CVectorGeo::CVectorGeo(double latitudeDegrees, double longitudeDegrees, double altitudeMeters)
        : m_latitudeDegrees(latitudeDegrees), m_longitudeDegrees(longitudeDegrees),
        m_altitudeMeters(altitudeMeters)
    {}

    CVectorGeo::CVectorGeo(const CVectorGeo &other)
        : m_latitudeDegrees(other.m_latitudeDegrees), m_longitudeDegrees(other.m_longitudeDegrees),
        m_altitudeMeters(other.m_altitudeMeters)
    {}

    CEcef CVectorGeo::toCartesian()
    {
        CEcef result;
		
		double phi = m_latitudeDegrees * Constants::DegToRad;
        double lambda = m_longitudeDegrees * Constants::DegToRad;
		double sphi = sin(phi);
		double cphi = 0;
		if (std::abs(m_latitudeDegrees) != 90)
			cphi = cos(phi);
			
		double n = Constants::EarthRadiusMeters/sqrt(1-Constants::e2 * CMath::square(sphi));
		
		double slambda = 0;
		if (m_longitudeDegrees != -180)
			slambda = sin(lambda);
		
		double clambda = 0;
		if (std::abs(m_longitudeDegrees) != 90)
			clambda = cos(lambda);
        
		double h = m_altitudeMeters;
		
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
        m_latitudeDegrees = 0;
        m_longitudeDegrees = 0;
        m_altitudeMeters = 0;
    }
	
	void CVectorGeo::print(std::ostream &stream)
    {
        stream << "v = " << std::endl;
	    stream << std::fixed;
        stream << "[" << m_latitudeDegrees << "]" << std::endl;
	    stream << "[" << m_longitudeDegrees << "]" << std::endl;
	    stream << "[" << m_altitudeMeters << "]" << std::endl;
    }

    CVectorGeo &CVectorGeo::operator =(const CVectorGeo &rhs)
    {
        if (this != &rhs)
        {
            m_latitudeDegrees = rhs.latitudeDegrees();
            m_longitudeDegrees = rhs.longitudeDegrees();
            m_altitudeMeters = rhs.altitudeMeters();
        }
        return *this;
    }

} // namespace BlackCore
