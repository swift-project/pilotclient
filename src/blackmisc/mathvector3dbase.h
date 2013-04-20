/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHVECTOR3DBASE_H
#define BLACKMISC_MATHVECTOR3DBASE_H

#include "blackmisc/basestreamstringifier.h"

namespace BlackMisc
{
namespace Math
{

class CMatrix3x3; // forward declaration
class CMatrix3x1; // forward declaration


/*!
 * \brief 3D vector base (x, y, z)
 */
template <class ImplClass> class CVector3DBase : public CBaseStreamStringifier<ImplClass>
{
protected:

    // using own value since Qt QVector3D stores internally as float
    double m_i; //!< Vector data i
    double m_j; //!< Vector data j
    double m_k; //!< Vector data k

    /*!
     * \brief Default constructor
     */
    CVector3DBase() {}

    /*!
     * \brief Constructor by values
     * \param i
     * \param j
     * \param k
     */
    CVector3DBase(double i, double j, double k) : m_i(i), m_j(j), m_k(k) {}

    /*!
     * \brief Constructor by value
     * \param value
     */
    CVector3DBase(double value) : m_i(value), m_j(value), m_k(value) {}

    /*!
     * \brief Copy constructor
     * \param otherVector
     */
    CVector3DBase(const CVector3DBase &otherVector) : m_i(otherVector.m_i), m_j(otherVector.m_j), m_k(otherVector.m_k) {}

    /*!
     * \brief String for converter
     * \return
     */
    virtual QString stringForConverter() const;

public:

    // getter and setters are implemented in the derived classes
    // as they have different names (x, i, north)

    /*!
     * \brief Virtual destructor
     */
    virtual ~CVector3DBase() {}

    /*!
     * \brief Set zeros
     */
    void setZero();

    /*!
     * \brief Set zeros
     */
    void fill(double value);

    /*!
     * \brief Get element
     * \param row
     * \return
     */
    double getElement(size_t row) const;

    /*!
     * \brief Set element
     * \param row
     * \param value
     */
    void setElement(size_t row, double value);

    /*!
     * \brief Operator []
     * \param row
     * \return
     */
    double operator[](size_t row) const { return this->getElement(row); }


    /*!
     * \brief Get element by ()
     * \param column
     * \return
     */
    double operator()(size_t row) const { return this->getElement(row); }

    /*!
     * \brief Equal operator ==
     * \param otherVector
     * \return
     */
    bool operator ==(const CVector3DBase &otherVector) const
    {
        if (this == &otherVector) return true;
        return this->m_i == otherVector.m_i &&
               this->m_j == otherVector.m_j &&
               this->m_k == otherVector.m_k;
    }

    /*!
     * \brief Unequal operator !=
     * \param otherVector
     * \return
     */
    bool operator !=(const CVector3DBase &otherVector) const
    {
        if (this == &otherVector) return false;
        return !((*this) == otherVector);
    }

    /*!
     * \brief Assigment operator =
     * \param otherVector
     * \return
     */
    CVector3DBase &operator =(const CVector3DBase &otherVector)
    {
        if (this == &otherVector) return *this; // Same object?
        this->m_i = otherVector.m_i;
        this->m_j = otherVector.m_j;
        this->m_k = otherVector.m_k;
        return (*this);
    }

    /*!
     * \brief Operator +=
     * \param otherVector
     * \return
     */
    CVector3DBase &operator +=(const CVector3DBase &otherVector)
    {
        this->m_i += otherVector.m_i;
        this->m_j += otherVector.m_j;
        this->m_k += otherVector.m_k;
        return (*this);
    }

    /*!
     * \brief Operator +
     * \param otherVector
     * \return
     */
    ImplClass operator +(const ImplClass &otherVector) const
    {
        ImplClass v;
        v += (*this);
        v += otherVector;
        return v;
    }

    /*!
     * \brief Operator -=
     * \param otherVector
     * \return
     */
    CVector3DBase &operator -=(const CVector3DBase &otherVector)
    {
        this->m_i -= otherVector.m_i;
        this->m_j -= otherVector.m_j;
        this->m_k -= otherVector.m_k;
        return (*this);
    }

    /*!
     * \brief Operator -
     * \param otherVector
     * \return
     */
    ImplClass operator -(const ImplClass &otherVector) const
    {
        ImplClass v;
        v += (*this);
        v -= otherVector;
        return v;
    }

    /*!
     * \brief Operator *=, just x*x, y*y, z*z neither vector nor dot product (like a matrix produc)
     * \param otherVector
     * \return
     */
    CVector3DBase &operator *=(const CVector3DBase &otherVector)
    {
        this->m_i *= otherVector.m_i;
        this->m_j *= otherVector.m_j;
        this->m_k *= otherVector.m_k;
        return (*this);
    }

    /*!
     * \brief Operator, just x*x, y*y, z*z neither vector nor dot product, (like a matrix produc)
     * \param otherVector
     * \return
     */
    ImplClass operator *(const ImplClass &otherVector) const
    {
        ImplClass v;
        v += (*this);
        v *= otherVector;
        return v;
    }

    /*!
     * \brief Operator /=, just x/x, y/y, z/z
     * \param otherVector
     * \return
     */
    CVector3DBase &operator /=(const CVector3DBase &otherVector)
    {
        this->m_i /= otherVector.m_i;
        this->m_j /= otherVector.m_j;
        this->m_k /= otherVector.m_k;
        return (*this);
    }

    /*!
     * \brief Operator, just x/x, y/y, z/z
     * \param otherVector
     * \return
     */
    ImplClass operator /(const ImplClass &otherVector) const
    {
        ImplClass v;
        v += (*this);
        v /= otherVector;
        return v;
    }

    /*!
     * \brief Dot product
     * \param otherVector
     * \return
     */
    double dotProduct(const ImplClass &otherVector) const;

    /*!
     * \brief Cross product
     * \param otherVector
     * \return
     */
    ImplClass crossProduct(const ImplClass &otherVector) const;

    /*!
     * \brief Matrix * this vector
     * \param matrix
     * \return
     */
    void matrixMultiplication(const CMatrix3x3 &matrix);

    /*!
     * \brief Reciporcal value
     * \param otherVector
     * \return
     */
    ImplClass reciprocalValues() const
    {
        ImplClass v;
        v.m_i = (1 / this->m_i);
        v.m_j = (1 / this->m_j);
        v.m_k = (1 / this->m_j);
        return v;
    }

    /*!
     * \brief Length
     * \return
     */
    double length()const
    {
        return this->m_i * this->m_j + this->m_k;
    }

    /*!
     * \brief Length squared
     * \return
     */
    double lengthSquared()const
    {
        return this->m_i * this->m_i + this->m_j * this->m_j + this->m_k * this->m_k;
    }

    /*!
     * \brief Converted to matrix
     * \return
     */
    CMatrix3x1 toMatrix3x1() const;

    /*!
     * \brief Magnitude
     * \return
     */
    double magnitude() const
    {
        return sqrt(this->lengthSquared());
    }
};

} // namespace

} // namespace

#endif // guard
