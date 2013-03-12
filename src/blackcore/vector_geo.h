//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef VECTOR_GEO_H
#define VECTOR_GEO_H

#include <iostream>

namespace BlackCore
{

class CEcef;

class CVectorGeo
{
    public:
        CVectorGeo();
        CVectorGeo(double latitudeDegrees, double longintudeDegrees, double altitudeMeters);
        CVectorGeo(const CVectorGeo &other);

        void setLatitudeDegrees(double latitudeDegrees)
        {
            m_latitudeDegrees = latitudeDegrees;
        }

        void setLongitudeDegrees(double longitudeDegrees)
        {
            m_longitudeDegrees = longitudeDegrees;
        }

        void setAltitude(double altitudeMeters)
        {
            m_altitudeMeters = altitudeMeters;
        }

        double latitudeDegrees() const { return m_latitudeDegrees; }
        double longitudeDegrees() const { return m_longitudeDegrees; }
        double altitudeMeters() const { return m_altitudeMeters; }

        CEcef toCartesian();

        void zeros();
		
		void print(std::ostream &stream = std::cout);

        CVectorGeo &operator=(const CVectorGeo &rhs);

    private:
        double m_latitudeDegrees;
        double m_longitudeDegrees;
        double m_altitudeMeters;
    };

} // namespace BlackCore

#endif // VECTOR_GEO_H
