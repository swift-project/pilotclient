/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHMATRIXBASE_H
#define BLACKMISC_MATHMATRIXBASE_H

#include "blackmisc/basestreamstringifier.h"
#include "blackmisc/mathvector3dbase.h"
#include <QGenericMatrix>

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief Base functionality of a matrix
 */
template<class ImplMatrix, int Rows, int Columns> class CMatrixBase : public BlackMisc::CBaseStreamStringifier<ImplMatrix>
{

protected:
    // no bug, Qt expects columns rows
    QGenericMatrix<Columns, Rows, double> m_matrix; //!< backing data

    /*!
     * \brief Conversion to string
     * \return
     */
    QString stringForConverter() const;

public:
    /*!
     * \brief Default constructor
     */
    CMatrixBase() : m_matrix() {}

    /*!
     * \brief Constructor with init value
     * \param fillValue
     */
    CMatrixBase(double fillValue) : m_matrix() { this->m_matrix.fill(fillValue);}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMatrixBase(const CMatrixBase &otherMatrix) : m_matrix(otherMatrix.m_matrix) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CMatrixBase() {}

    /*!
     * \brief Equal operator ==
     * \param otherMatrix
     * \return
     */
    bool operator ==(const ImplMatrix &otherMatrix) const
    {
        if (this == &otherMatrix) return true;
        return this->m_matrix == otherMatrix.m_matrix;
    }

    /*!
     * \brief Unequal operator !=
     * \param otherMatrix
     * \return
     */
    bool operator !=(const ImplMatrix &otherMatrix) const
    {
        if (this == &otherMatrix) return false;
        return !((*this) == otherMatrix);
    }

    /*!
     * \brief Assigment operator =
     * \param multiply
     * \return
     */
    CMatrixBase &operator =(const CMatrixBase &otherMatrix)
    {
        if (this == &otherMatrix)  return *this; // Same object?
        this->m_matrix = otherMatrix.m_matrix;
        return (*this);
    }

    /*!
     * \brief Operator *=
     * \param factor
     * \return
     */
    CMatrixBase &operator *=(double factor)
    {
        this->m_matrix *= factor;
        return (*this);
    }

    /*!
     * \brief Operator *
     * \param factor
     * \return
     */
    CMatrixBase &operator *(double factor)
    {
        ImplMatrix m(0.0);
        m += (*this);
        m *= factor;
        return m;
    }

    /*!
     * \brief Multiply with 3D vector operator *
     * \param vector
     * \return
     */
    template<class ImplVector> ImplVector operator*(const ImplVector vector) const;

    /*!
     * \brief Operator /=
     * \param factor
     * \return
     */
    CMatrixBase &operator /=(double factor)
    {
        this->m_matrix /= factor;
        return (*this);
    }

    /*!
     * \brief Operator /
     * \param factor
     * \return
     */
    CMatrixBase &operator /(double factor)
    {
        ImplMatrix m(0.0);
        m += (*this);
        m /= factor;
        return m;
    }

    /*!
     * \brief Operator +=
     * \param otherMatrix
     * \return
     */
    CMatrixBase &operator +=(const CMatrixBase &otherMatrix)
    {
        this->m_matrix += otherMatrix.m_matrix;
        return (*this);
    }

    /*!
     * \brief Operator +
     * \param otherMatrix
     * \return
     */
    ImplMatrix operator +(const ImplMatrix &otherMatrix) const
    {
        ImplMatrix m(0.0);
        m += (*this);
        m += otherMatrix;
        return m;
    }

    /*!
     * \brief Operator -=
     * \param otherMatrix
     * \return
     */
    CMatrixBase &operator -=(const CMatrixBase &otherMatrix)
    {
        this->m_matrix -= otherMatrix.m_matrix;
        return (*this);
    }

    /*!
     * \brief Operator -
     * \param otherMatrix
     * \return
     */
    ImplMatrix operator -(const ImplMatrix &otherMatrix) const
    {
        ImplMatrix m(0.0);
        m += (*this);
        m -= otherMatrix;
        return m;
    }

    /*!
     * \brief Is identity matrix?
     * \return
     */
    bool isIdentity() const
    {
        return this->m_matrix.isIdentity();
    }

    /*!
     * \brief Set as identity matrix
     * \return
     */
    void setToIdentity()
    {
        this->m_matrix.setToIdentity();
    }

    /*!
     * \brief Fills the matrix with random elements
     */
    void setRandom();

    /*!
     * \brief All values to zero
     */
    void setZero() { this->m_matrix.fill(0.0); }

    /*!
     * \brief isZero
     * \return
     */
    bool isZero() const;

    /*!
     * \brief Set a dedicated value
     * \param value
     */
    void fill(double value) { this->m_matrix.fill(value); }

    /*!
     * \brief Get element
     * \param row
     * \param column
     * \return
     */
    double getElement(size_t row, size_t column) const;

    /*!
     * \brief Get element
     * \param row
     * \param column
     * \param value
     */
    void setElement(size_t row, size_t column, double value);

    /*!
     * \brief Get element by ()
     * \param column
     * \return
     */
    double &operator()(size_t row, size_t column)
    {
        this->checkRange(row, column);
        return this->m_matrix(row, column);
    }

    /*!
     * \brief Get element by ()
     * \param column
     * \return
     */
    double operator()(size_t row, size_t column) const
    {
        return this->getElement(row, column);
    }

private:
    /*!
     * \brief Check range of row / column
     * \param row
     * \param column
     * \throws std::range_error if index out of bounds
     */
    void checkRange(size_t row, size_t column) const;

};

} // namespace

} // namespace

#endif // guard
