/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHVECTOR3DBASE_H
#define BLACKMISC_MATHVECTOR3DBASE_H

#include "blackmisc/basestreamstringifier.h"
#include <QVector3D>

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

    QVector3D m_vector; //!< Vector data

    /*!
     * \brief Default constructor
     */
    CVector3DBase() : m_vector() {}

    /*!
     * \brief Constructor by values
     * \param i
     * \param j
     * \param k
     */
    CVector3DBase(qreal i, qreal j, qreal k) : m_vector(i, j, k) {}

    /*!
     * \brief Constructor by value
     * \param value
     */
    CVector3DBase(qreal value) : m_vector(value, value, value) {}

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
    void fill(qreal value);

    /*!
     * \brief Get element
     * \param row
     * \return
     */
    qreal getElement(size_t row) const;

    /*!
     * \brief Set element
     * \param row
     * \param value
     */
    void setElement(size_t row, qreal value);

    /*!
     * \brief Operator []
     * \param row
     * \return
     */
    qreal operator[](size_t row) const { return this->getElement(row); }


    /*!
     * \brief Get element by ()
     * \param column
     * \return
     */
    qreal operator()(size_t row) const { return this->getElement(row); }

    /*!
     * \brief Equal operator ==
     * \param otherVector
     * \return
     */
    bool operator ==(const CVector3DBase &otherVector) const
    {
        if (this == &otherVector) return true;
        return this->m_vector == otherVector.m_vector;
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
        if (this == &otherVector)  return *this; // Same object?
        this->m_vector = otherVector.m_vector;
        return (*this);
    }

    /*!
     * \brief Operator +=
     * \param otherVector
     * \return
     */
    CVector3DBase &operator +=(const CVector3DBase &otherVector)
    {
        this->m_vector += otherVector.m_vector;
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
        this->m_vector -= otherVector.m_vector;
        return (*this);
    }

    /*!
     * \brief Operator +
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
        this->m_vector *= otherVector.m_vector;
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
        this->m_vector *= otherVector.reciprocalValues().m_vector;
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
    qreal dotProduct(const ImplClass &otherVector) const
    {
        return QVector3D::dotProduct(this->m_vector, otherVector.m_vector);
    }

    /*!
     * \brief Cross product
     * \param otherVector
     * \return
     */
    ImplClass crossProduct(const ImplClass &otherVector) const
    {
        ImplClass v;
        v.m_vector = QVector3D::crossProduct(this->m_vector, otherVector.m_vector);
        return v;
    }

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
        v.m_vector.setX(1 / this->m_vector.x());
        v.m_vector.setY(1 / this->m_vector.y());
        v.m_vector.setZ(1 / this->m_vector.z());
        return v;
    }

    /*!
     * \brief Length
     * \return
     */
    qreal length()const
    {
        return this->m_vector.length();
    }

    /*!
     * \brief Length squared
     * \return
     */
    qreal lengthSquared()const
    {
        return this->m_vector.lengthSquared();
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
    qreal magnitude() const
    {
        return sqrt(this->lengthSquared());
    }
};

} // namespace

} // namespace

#endif // guard
