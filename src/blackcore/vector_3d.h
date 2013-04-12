//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <qglobal.h>

#ifndef VECTOR_3D_H
#define VECTOR_3D_H

namespace BlackCore
{

	class CMatrix3D;

	class CVector3D
	{
	public:
		CVector3D();
		CVector3D(double x, double y, double z);
		CVector3D( const CVector3D & other);

		double X() const {return v[0];}

		double Y() const {return v[1];}

		double Z() const {return v[2];}

		void setX(const double num) { v[0] = num; }

		void setY(const double num) { v[1] = num; }

		void setZ(const double num) { v[2] = num; }

		double getElement(qint8 row) const;

		void print();

		void zeros();

		CVector3D & operator +=(const CVector3D &rhs);
		CVector3D & operator -=(const CVector3D &rhs);
        CVector3D & operator = (const CVector3D &rhs);

        CVector3D operator +(const CVector3D &rhs);
        CVector3D operator -(const CVector3D &rhs);

        bool operator ==(const CVector3D &rhs);
        bool operator !=(const CVector3D &rhs);

        //double crossProduct(qint32 );


		CVector3D & operator *=(const CVector3D &rhs);
		CVector3D operator *( const CVector3D &rhs);

		CVector3D & operator *=( const double rhs);
		CVector3D operator *( const double rhs);

		CVector3D & operator /=( const double rhs);
		CVector3D operator /( const double rhs);

		double magnitude();

	protected:
		double v[3];

		friend class CMatrix3D;
	};

} //! namespace BlackCore

#endif // VECTOR_3D_H
