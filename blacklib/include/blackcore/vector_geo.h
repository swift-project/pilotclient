//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef VECTOR_GEO_H
#define VECTOR_GEO_H

namespace BlackCore
{

class CEcef;

class CVectorGeo
{
    public:
        CVectorGeo();
        CVectorGeo(double lat, double lon, double alt);
        CVectorGeo( const CVectorGeo &other );

        void setLatitude( double latitude)
        {
            m_latitude = latitude;
        }

        void setLongitude( double longitude)
        {
            m_longitude = longitude;
        }

        void setAltitude( double altitude)
        {
            m_altitude = altitude;
        }

        double latitude() const {return m_latitude;}
        double longitude() const {return m_longitude;}
        double altitude() const {return m_altitude;}

        CEcef toCartesian();

        void zeros();
		
		void print();

        CVectorGeo & operator = (const CVectorGeo &rhs);

    private:
        double m_latitude;
        double m_longitude;
        double m_altitude;
    };

} // namespace BlackCore

#endif // VECTOR_GEO_H
