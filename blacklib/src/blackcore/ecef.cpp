//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/
//!
//! This file incorporates work covered by the following copyright and  
//! permission notice:
//!
//! Copyright (c) Charles Karney (2008-2011) <charles@karney.com> and licensed
//! under the MIT/X11 License. For more information, see
//! http://geographiclib.sourceforge.net/

#include "blackcore/matrix_3d.h"
#include "blackcore/ned.h"
#include "blackcore/ecef.h"
#include "blackcore/math.h"
#include "blackcore/constants.h"

namespace BlackCore
{

CEcef::CEcef()
{
}

CEcef::CEcef(double X, double Y, double Z)
    : CVector3D(X, Y, Z)
{
}

CNed CEcef::toNED(const CVectorGeo &pos)
{
    CNed result;
	
	double angle = - ( pos.latitudeDegrees() * Constants::DegToRad ) - Constants::PI/2;
	
	CMatrix3D dcm1;
	CMatrix3D dcm2;
	CMatrix3D dcm3;
	CMatrix3D DCM;
	dcm1.zeros();
	dcm2.zeros();
	dcm3.zeros();
	DCM.zeros();
	
	dcm1(0,0) = 1;
	dcm1(1,1) = 1;
	dcm1(1,2) = 0;
	dcm1(2,1) = 0;
	dcm1(2,2) = 1;

	dcm2(0,0) = cos( angle );
	dcm2(0,2) = -sin( angle );
	dcm2(1,1) = 1;
	dcm2(2,0) = sin( angle );
	dcm2(2,2) = cos( angle );

	angle = pos.longitudeDegrees() * Constants::DegToRad;

	dcm3(0,0) = cos(angle );
	dcm3(0,1) = sin(angle );
	dcm3(1,0) = -sin(angle );
	dcm3(1,1) = cos(angle );
	dcm3(2,2) = 1;
	
	DCM = dcm1 * dcm2 * dcm3;
	
	CVector3D tempResult = DCM * (*this);
	
	result.setNorth(tempResult.X());
	result.setEast(tempResult.Y());
	result.setDown(0);
	result.setPosition(pos);
	
	return result;
}

void CEcef::operator= (const CVector3D &rhs)
{
	v[0] = rhs.X();
	v[1] = rhs.Y();
	v[2] = rhs.Z();
}

CVectorGeo CEcef::toGeodetic()
{
	CVectorGeo result;

	double R = CMath::hypot(v[0], v[1]);
	
	double slam = 0;
	double clam = 1;
	
	if (R)
	{
		slam = v[1] / R;
		clam = v[0] / R;
	}
	
	//! Calculate the distance to the earth
	double h = CMath::hypot(R, v[2]);
	
	double sphi = 0;
	double cphi = 0;
	
	double p = CMath::square(R / Constants::EarthRadiusMeters);
	double q = Constants::e2m * CMath::square(v[2] / Constants::EarthRadiusMeters);
	double r = (p + q - Constants::e4) / 6.0;
	
	if ( !(Constants::e4 * q == 0 && r <= 0) )
	{
		//! Avoid possible division by zero when r = 0 by multiplying
		//! equations for s and t by r^3 and r, resp.
		
		double S = Constants::e4 * p * q / 4; //! S = r^3 * s
		double r2 = CMath::square(r);
		double r3 = r * r2;
		double disc = S * (2 * r3 + S);
		double u = r;
		
		if (disc >= 0) 
		{
			double T3 = S + r3;
			/*!
			  Pick the sign on the sqrt to maximize abs(T3). This minimizes
			  loss of precision due to cancellation. The result is unchanged
			  because of the way the T is used in definition of u.
			*/
			T3 += T3 < 0 ? -sqrt(disc) : sqrt(disc); // T3 = (r * t)^3
			
			//!N.B. cubicRootReal always returns the real root. cubicRootReal(-8) = -2.
			double T = CMath::cubicRootReal(T3);
			
			//! T can be zero; but then r2 / T -> 0.
			u += T + (T != 0 ? r2 / T : 0);
		}
		else
		{
			//! T is complex, but the way u is defined the result is real.
			double ang = atan2(sqrt(-disc), -(S + r3));
			/*!
			  There are three possible cube roots. We choose the root which
			  avoids cancellation. Note that disc < 0 implies that r < 0.
			*/
			u += 2 * r * cos(ang / 3);
		}
		
		//! This is garanteed positive
		double V = sqrt(CMath::square(u) + Constants::e4 * q);
		
		/*!
		  Avoid loss of accuracy when u < 0. Underflow doesn't occur in
		  e4 * q / (v - u) because u ~ e^4 when q is small and u < 0.
		*/
		double uv = u < 0 ? Constants::e4 * q / (V - u) : u + V; //! u+v, guaranteed positive
		
		//! Need to guard against w going negative due to roundoff in uv - q.
		double w = std::max(double(0), Constants::e2absolute * (uv - q) / (2 * V));
		
		/*!
		  Rearrange expression for k to avoid loss of accuracy due to
		  subtraction. Division by 0 not possible because uv > 0, w >= 0.
		*/
		double k = uv / (sqrt(uv + CMath::square(w)) + w);
		double k1 = k;
		double k2 = k + Constants::e2;
		double d = k1 * R / k2;
		double H = CMath::hypot((v[2])/k1, R/k2);
		
		sphi = (v[2] / k1) / H;
		cphi = (R / k2) / H;
		
		h = (1 - Constants::e2m/k1) * CMath::hypot(d, v[2]); 
	}
	else //! e4 * q == 0 && r <= 0
	{
		/*!
		  This leads to k = 0 (oblate, equatorial plane) and k + e^2 = 0
		  (prolate, rotation axis) and the generation of 0/0 in the general
		  formulas for phi and h. using the general formula and division by 0
		  in formula for h. So handle this case by taking the limits:
		  f > 0: z -> 0, k -> e2 * sqrt(q)/sqrt(e4 - p)
		  f < 0: R -> 0, k + e2 -> - e2 * sqrt(q)/sqrt(e4 - p)
		*/
		double zz = sqrt((Constants::e4 - p) / Constants::e2m);
		double xx = sqrt( p );
		double H = CMath::hypot(zz,xx);
		sphi = zz / H;
		cphi = xx / H;
		if (v[2] < 0) sphi = -sphi; // for tiny negative Z (not for prolate)
		h = - Constants::EarthRadiusMeters * (Constants::e2m) * H / Constants::e2absolute;
	}
	
	double lat = atan2(sphi, cphi) * Constants::RadToDeg;
	
	//! Negative signs return lon in [-180, 180).
	double lon = -atan2(-slam, clam) * Constants::RadToDeg;
	
	result.setLongitudeDegrees(lon);
    result.setLatitudeDegrees(lat);
    result.setAltitude(h);
	
	return result;
		

/*
    CVectorGeo result;

    double Xpow2plusYpow2 = CMath::square(v[0])+CMath::square(v[1]);

    if( Xpow2plusYpow2 + CMath::square(v[2]) < 25 ) {

        // This function fails near the geocenter region, so catch that special case here.
        // Define the innermost sphere of small radius as earth center and return the
        // coordinates 0/0/-EQURAD. It may be any other place on geoide's surface,
        // the Northpole, Hawaii or Wentorf. This one was easy to code ;-)

        result.setLongitude( 0.0 );
        result.setLatitude( 0.0 );
        result.setAltitude( -Constants::EarthRadius );

        return result;

      }

    double R = CMath::hypot(v[0], v[1]);
    double p = CMath::square(R / Constants::EarthRadius);
    double q = CMath::square(v[2] / Constants::EarthRadius)*(1-e2);
    double r = 1/6.0*(p+q-e4);
    double s = e4*p*q/(4*CMath::cubic(r));

    if( s >= -2.0 && s <= 0.0 )
        s = 0.0;

    double t = pow(1+s+sqrt(s*(2+s)), 1/3.0);
    double u = r*(1+t+1/t);
    double vt = sqrt(u*u+e4*q);
    double w = e2*(u+vt-q)/(2*vt);
    double k = sqrt(u+vt+w*w)-w;
    double D = k*R/(k+e2);

    result.setLongitude(2*atan2(v[1], v[0]+R)*RadToDeg);
    double hypot = CMath::hypot(D, v[2]); //sqrt(D*D+v[2]*v[2]);
    result.setLatitude(2*atan2(v[2], D+hypot)*RadToDeg);
    result.setAltitude((k+e2-1)*hypot/k*M_TO_FT);

    return result;*/
}

} // namespace BlackCore
