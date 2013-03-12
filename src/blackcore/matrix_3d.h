//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MATRIX_3D_H
#define MATRIX_3D_H

#include "blackcore/vector_3d.h"
#include "blackcore/ecef.h"

namespace BlackCore
{

class CMatrix3D
{
public:
    CMatrix3D();

    CMatrix3D(const CMatrix3D & other);
	
	/*!
	  Basic Matrix functions
	*/
	
	//! Fills the matrix with random elements
    void random();

	//! Calculates the determinant of the matrix
    double determinant();

	//! Returns the inverse matrix
    CMatrix3D inverse();

	//! Sets all elements to zero
    void zeros();

	//! Prints the matrix to stdout
    void print();

	//! Returns an element
    /*!
      \param row Specifies elements row
	  \param column Specifies elements column
	  \return Returns element of [row, column]
    */
    double getElement(qint8 row, qint8 column) const;

	//! Sets a matrix element
    /*!
      \param row Specifies elements row
	  \param column Specifies elements column
	  \param value Specifies the new elements value
    */
    void setElement(qint8 row, qint8 column, double value);
	
	/*!
	  Operators
	*/

    CMatrix3D & operator +=(const CMatrix3D &rhs);
    CMatrix3D & operator -=(const CMatrix3D &rhs);

    CMatrix3D & operator = (const CMatrix3D &rhs);

    CMatrix3D operator +(const CMatrix3D &rhs);
    CMatrix3D operator -(const CMatrix3D &rhs);

    bool operator ==(const CMatrix3D &rhs);
    bool operator !=(const CMatrix3D &rhs);

    CMatrix3D & operator *=(const CMatrix3D &rhs);
    CMatrix3D operator *(const CMatrix3D &rhs);

    CVector3D operator * ( const CVector3D &rhs);
    CEcef     operator * ( const CEcef &rhs);

    double& operator() (const qint8 row, const qint8 column);

private:
    double m[3][3];
};

} // namespace BlackCore

#endif // MATRIX_3D_H
