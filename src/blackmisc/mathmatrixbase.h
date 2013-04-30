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
private:
    /*!
     * \brief Easy access to derived class (CRTP template parameter)
     * \return
     */
    ImplMatrix const *derived() const
    {
        return static_cast<ImplMatrix const *>(this);
    }

    /*!
     * \brief Easy access to derived class (CRTP template parameter)
     * \return
     */
    ImplMatrix *derived()
    {
        return static_cast<ImplMatrix *>(this);
    }

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
     * \brief Copy constructor
     * \param otherMatrix
     */
    CMatrixBase(const CMatrixBase &otherMatrix) : m_matrix(otherMatrix.m_matrix) {}

    /*!
     * \brief Fill with value
     * \param fillValue
     */
    explicit CMatrixBase(double fillValue) : m_matrix()
    {
        this->fill(fillValue);
    }

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
     * \param otherMatrix
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
    ImplMatrix operator *(double factor) const
    {
        ImplMatrix m = *derived();
        m *= factor;
        return m;
    }

    /*!
     * \brief Operator to support commutative multiplication
     * \param factor
     * \param otherMatrix
     * \return
     */
    friend ImplMatrix operator *(double factor, const ImplMatrix &otherMatrix)
    {
        return otherMatrix * factor;
    }

    /*!
     * \brief Multiply with 3D vector operator *
     * \param matrix
     * \return
     */
    template<class ImplVector> ImplVector operator*(const ImplVector matrix) const;

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
    ImplMatrix operator /(double factor) const
    {
        ImplMatrix m = *derived();
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
        ImplMatrix m = *derived();
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
        ImplMatrix m = *derived();
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
     * \brief Is identity matrix? Epsilon considered.
     * \return
     */
    bool isIdentityEpsilon() const
    {
        ImplMatrix m = *derived();
        m.round();
        return m.isIdentity();
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
     * \brief Is zero
     * \return
     */
    bool isZero() const;

    /*!
     * \brief Is identity matrix? Epsilon considered.
     * \return
     */
    bool isZeroEpsilon() const
    {
        ImplMatrix m = *derived();
        m.round();
        return m.isZero();
    }

    /*!
     * \brief All values equal, if so matirx is not invertible
     * \return
     */
    bool allValuesEqual() const;

    /*!
     * \brief Set a dedicated value
     * \param value
     */
    void fill(double value) { this->m_matrix.fill(value); }

    /*!
     * \brief Round all values
     */
    void round();

    /*!
     * \brief Return a rounded matrix
     * \return
     */
    ImplMatrix roundedMatrix() const
    {
        ImplMatrix m = *derived();
        m.round();
        return m;
    }

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
     * \brief Get element by operator () modifying
     * \param row
     * \param column
     * \return
     */
    double &operator()(size_t row, size_t column)
    {
        this->checkRange(row, column);
        return this->m_matrix(row, column);
    }

    /*!
     * \brief Get element by operator () read only
     * \param row
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
