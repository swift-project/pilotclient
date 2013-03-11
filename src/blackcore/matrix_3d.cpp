#include <iostream>
#include "blackmisc/debug.h"
#include "blackcore/vector_3d.h"
#include "blackcore/matrix_3d.h"

/* TODO
 *
class Vector2D
{
private:
   Vector data;//der macht copy-ctor, dtor und op=
public:
   class Zeilenproxy
   {
      private:
         Vector2D& v2d;
         int zeile;
      public:
         Zeilenproxy(Vector2D& _v2d,int _y)
         :v2d(_v2d),y(_y)
         {
         }
         double& operator[](int x)
         {
            return v2d.data[y*SIZEX+x];//kacke, nur demo
         }
   }
   ZeilenProxy operator[](int y)
   {
      return Zeilenproxy(*this,y);
   }
};
*/

namespace BlackCore
{

CMatrix3D::CMatrix3D()
{
    zeros();
}

CMatrix3D::CMatrix3D(const CMatrix3D & other)
{
    zeros();
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            m[i][j] = other.getElement(i, j);
        }
    }
}

void CMatrix3D::random()
{

}

double CMatrix3D::determinant()
{
    double determinant =    m[0][0]*m[1][1]*m[2][2] +
                            m[0][1]*m[1][2]*m[2][0] +
                            m[0][2]*m[1][0]*m[2][1] -
                            m[0][1]*m[1][0]*m[2][2] -
                            m[0][2]*m[1][1]*m[2][0] -
                            m[0][0]*m[1][2]*m[2][1];

    return determinant;
}

CMatrix3D CMatrix3D::inverse()
{
    CMatrix3D result;
    double det = determinant();

    if (!det)
        return result;

    double invdet = 1 / determinant();

    result.m[0][0] = (   m[1][1]*m[2][2] - m[1][2]*m[2][1] ) * invdet;
    result.m[0][1] = ( - m[0][1]*m[2][2] + m[0][2]*m[2][1] ) * invdet;
    result.m[0][2] = (   m[0][1]*m[1][2] - m[0][2]*m[1][1] ) * invdet;
    result.m[1][0] = ( - m[1][0]*m[2][2] + m[1][2]*m[2][0] ) * invdet;
    result.m[1][1] = (   m[0][0]*m[2][2] - m[0][2]*m[2][0] ) * invdet;
    result.m[1][2] = ( - m[0][0]*m[1][2] + m[0][2]*m[1][0] ) * invdet;
    result.m[2][0] = (   m[1][0]*m[2][1] - m[1][1]*m[2][0] ) * invdet;
    result.m[2][1] = ( - m[0][0]*m[2][1] + m[0][1]*m[2][0] ) * invdet;
    result.m[2][2] = (   m[0][0]*m[1][1] - m[0][1]*m[1][0] ) * invdet;

    return result;
}

void CMatrix3D::zeros()
{
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            m[i][j] = 0;
        }
    }
}

void CMatrix3D::print()
{
    std::cout << m[0][0] << "  " << m[0][1] << "  " << m[0][2] << std::endl;
    std::cout << m[1][0] << "  " << m[1][1] << "  " << m[1][2] << std::endl;
    std::cout << m[2][0] << "  " << m[2][1] << "  " << m[2][2] << std::endl;
}

double CMatrix3D::getElement(qint8 row, qint8 column) const
{
    bAssert (row < 3 || column < 3);

    return m[row][column];
}

void CMatrix3D::setElement(qint8 row, qint8 column, double value)
{
    bAssert (row < 3 || column < 3);

    m[row][column] = value;
}

CMatrix3D & CMatrix3D::operator +=(const CMatrix3D &rhs)
{
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            m[i][j] += rhs.getElement(i, j);
        }
    }
    return *this;
}

CMatrix3D & CMatrix3D::operator -=(const CMatrix3D &rhs)
{
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            m[i][j] -= rhs.getElement(i, j);
        }
    }
    return *this;
}

CMatrix3D& CMatrix3D::operator = (const CMatrix3D &rhs)
{
    if (this != &rhs)
    {
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                m[i][j] = rhs.getElement(i, j);
            }
        }
    }
    return *this;
}

CMatrix3D CMatrix3D::operator + (const CMatrix3D &rhs)
{
    CMatrix3D helper = *this;
    helper += rhs;
    return helper;
}

CMatrix3D CMatrix3D::operator - (const CMatrix3D &rhs)
{
    CMatrix3D helper = *this;
    helper -= rhs;
    return helper;
}

bool CMatrix3D::operator == (const CMatrix3D &rhs)
{
    bool isEqual = true;
    for(int i=0; i<3 && isEqual; ++i)
    {
        for(int j=0; j<3 && isEqual; ++j)
        {
            isEqual = (m[i][j] == rhs.getElement(i, j));
        }
    }
    return isEqual;
}

bool CMatrix3D::operator != (const CMatrix3D &rhs)
{
    return !(*this == rhs);
}

CMatrix3D & CMatrix3D::operator *= (const CMatrix3D &rhs)
{
    CMatrix3D helper(*this);
    for(qint32 column = 0; column < 3; ++column)
    {
        for(qint32 row = 0; row < 3; ++row )
        {
            m[row][column] = helper.getElement(row,0) * rhs.getElement(0, column);
            for(qint32 line = 1; line < 3; ++line)
            {
                m[row][column] += helper.getElement(row,line) * rhs.getElement(line, column);
            }
        }
    }
    return *this;
}

CMatrix3D CMatrix3D::operator * (const CMatrix3D &rhs)
{
    CMatrix3D helper(*this);
    helper *= rhs;
    return helper;
}

CVector3D CMatrix3D::operator * ( const CVector3D &rhs)
{
    CVector3D result;
    for (qint32 i = 0; i < 3; ++i)
    {
        for (qint32 j = 0; j < 3; ++j)
        {
            result.v[i] += m[i][j] * rhs.v[j];
        }
    }

    return result;
}

CEcef CMatrix3D::operator * ( const CEcef &rhs)
{
    CEcef result;
    for (qint32 i = 0; i < 3; ++i)
    {
        for (qint32 j = 0; j < 3; ++j)
        {
            result.v[i] += m[i][j] * rhs.v[j];
        }
    }

    return result;
}

double& CMatrix3D::operator() (const qint8 row, const qint8 column)
{
    return m[row][column];
}

} // namespace BlackCore
