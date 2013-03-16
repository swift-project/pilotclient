//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef ECEF_H
#define ECEF_H

#include "vector_3d.h"

namespace BlackCore
{

class CNed;
class CVectorGeo;

class CEcef : public CVector3D
{
public:
    CEcef();
    CEcef(double X, double Y, double Z);

	//! Converts this velocity vector into the NED format
    /*!
      \param pos This position is needed for correct calculation
	  \return velocity in NED coordinate system
    */
    CNed toNED(const CVectorGeo &pos);
	
	//! Converts this position vector into the geodetic format
    /*!
	  \return Position in latitude, longitude and altitude
    */
	CVectorGeo toGeodetic();

	//! Assignment operator 
    void operator= (const CVector3D &rhs);
};

} // namespace BlackCore

#endif // ECEF_H
