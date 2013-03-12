//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef PLANE_H
#define PLANE_H

#include <QString>

namespace BlackCore {

	class ISimulator;
	class CInterpolator;

	class CPlane
	{
	public:

		enum ESquawkMode {Standby   = 'S',
						  Charlie   = 'N',
						  Ident     = 'Y'};

		CPlane();
		CPlane(const QString &callsign, ISimulator *driver);



		void addPosition(const CVectorGeo &position, double groundVelocity, double heading, double pitch, double bank);

		//! Returns the callsign of the multiplayer plane
		/*!
		  \return callsign.
		*/
		inline QString& Callsign() { return m_callsign; }
	
		void render();
	
		double getLastUpdateTime();

	private:

		QString			m_callsign;

		CInterpolator	*m_interpolator;

		ISimulator		*m_driver;
	};

} // namespace BlackCore

#endif // PLANE_H
