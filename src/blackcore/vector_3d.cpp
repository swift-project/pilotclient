
#include "blackcore/vector_3d.h"
#include "blackcore/matrix_3d.h"
#include "blackcore/vector_geo.h"
#include "blackcore/constants.h"
#include "blackmisc/debug.h"
#include <iostream>
#include <math.h>

namespace BlackCore
{

CVector3D::CVector3D()
{
    zeros();
}

CVector3D::CVector3D(double x, double y, double z)
{
    zeros();
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

CVector3D::CVector3D(const CVector3D &other)
{
    zeros();
    for (int i=0; i<3; ++i)
    {
        v[i] = other.getElement(i);
    }
}

void CVector3D::print()
{
    std::cout << "v = " << std::endl;
	std::cout << std::fixed;
    for (qint32 i = 0; i < 3; ++i)
    {
        std::cout << "[" << v[i] << "]" << std::endl;
    }
}

void CVector3D::zeros()
{
    for (qint32 i = 0; i < 3; ++i)
    {
        v[i] = 0;
    }
}

double CVector3D::magnitude()
{
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

double CVector3D::getElement(qint8 row) const
{
    Q_ASSERT(row < 3);

    return v[row];
}

CVector3D &CVector3D::operator +=(const CVector3D &rhs)
{
    for (int i=0; i<3; ++i)
    {
        v[i] += rhs.getElement(i);
    }
    return *this;
}

CVector3D &CVector3D::operator -=(const CVector3D &rhs)
{
    for (int i=0; i<3; ++i)
    {
        v[i] -= rhs.getElement(i);
    }
    return *this;
}

CVector3D &CVector3D::operator =(const CVector3D &rhs)
{
    if (this != &rhs)
    {
        for (int i=0; i<3; ++i)
        {
            v[i] = rhs.getElement(i);
        }
    }
    return *this;
}

CVector3D CVector3D::operator + (const CVector3D &rhs)
{
    CVector3D helper = *this;
    helper += rhs;
    return helper;
}

CVector3D CVector3D::operator - (const CVector3D &rhs)
{
    CVector3D helper = *this;
    helper -= rhs;
    return helper;
}

bool CVector3D::operator == (const CVector3D &rhs)
{
    bool isEqual = true;
    for(int i=0; i<3 && isEqual; ++i)
    {
        isEqual = (v[i] == rhs.getElement(i));
    }
    return isEqual;
}

bool CVector3D::operator != (const CVector3D &rhs)
{
    return !(*this == rhs);
}

CVector3D & CVector3D::operator *= (const CVector3D &rhs)
{
    CVector3D helper(*this);
    for(qint32 row = 0; row < 3; ++row )
    {
        v[row] = helper.getElement(row) * rhs.getElement(row);
    }
    return *this;
}

CVector3D CVector3D::operator * (const CVector3D &rhs)
{
    CVector3D helper(*this);
    helper *= rhs;
    return helper;
}

CVector3D &CVector3D::operator *= ( const double rhs)
{
    CVector3D helper(*this);
    for(qint32 row = 0; row < 3; ++row )
    {
        v[row] = helper.getElement(row) * rhs;
    }
    return *this;
}

CVector3D CVector3D::operator * (const double rhs)
{
    CVector3D helper(*this);
    helper *= rhs;
    return helper;
}

CVector3D &CVector3D::operator /= ( const double rhs)
{
    CVector3D helper(*this);
    for(qint32 row = 0; row < 3; ++row )
    {
        v[row] = helper.getElement(row) / rhs;
    }
    return *this;
}

CVector3D CVector3D::operator / (const double rhs)
{
    CVector3D helper(*this);
    helper /= rhs;
    return helper;
}

} // namespace BlackCore
