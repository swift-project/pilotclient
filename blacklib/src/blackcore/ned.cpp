//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackcore/matrix_3d.h"
#include "blackcore/ecef.h"
#include "blackcore/ned.h"

#include "blackcore/constants.h"

namespace BlackCore
{

	CNed::CNed()
	{
		zeros();
	}

	CNed::CNed(CVectorGeo &pos, double N, double E, double D)
		: CVector3D(N, E, D), m_position(pos)
	{
	}

	CEcef CNed::toECEF()
	{
		double angle = - ( m_position.latitudeDegrees() * Constants::DegToRad ) - Constants::PI/2;

		CMatrix3D dcm1;
		CMatrix3D dcm2;
		CMatrix3D dcm3;
		CMatrix3D DCM;
		CMatrix3D invDCM;
		dcm1.zeros();
		dcm2.zeros();
		dcm3.zeros();

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

		angle = m_position.longitudeDegrees() * Constants::DegToRad;

		dcm3(0,0) = cos(angle );
		dcm3(0,1) = sin(angle );
		dcm3(1,0) = -sin(angle );
		dcm3(1,1) = cos(angle );
		dcm3(2,2) = 1;

		DCM = dcm1 * dcm2 * dcm3;

		invDCM.zeros();
		invDCM = DCM.inverse();

		CVector3D tempResult = invDCM * (*this);
		CEcef result;

		result.setX(tempResult.X());
		result.setY(tempResult.Y());
		result.setZ(tempResult.Z());

		return result;
}

} // namespace BlackCore

