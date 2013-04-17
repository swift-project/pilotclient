/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHMATRIXBASE_H
#define BLACKMISC_MATHMATRIXBASE_H

#include "blackmisc/basestreamstringifier.h"
#include <QGenericMatrix>

namespace BlackMisc
{
namespace Math
{

/*!
 * \brief Base functionality of a matrix
 */
template<class ImplMatrix, int Rows, int Columns> class CMatrixBase :
    public BlackMisc::CBaseStreamStringifier<ImplMatrix>
{
protected:
    QGenericMatrix<Rows, Columns, qreal> m_matrix; //!< backing data

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
    CMatrixBase(qreal fillValue) : m_matrix() { this->m_matrix.fill(fillValue);}

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
     * \param multiply
     * \return
     */
    CMatrixBase &operator *=(const CMatrixBase &otherMatrix)
    {
        this->m_matrix = this->m_matrix * otherMatrix.m_matrix;
        return (*this);
    }

    /*!
     * \brief Operator *
     * \param multiply
     * \return
     */
    ImplMatrix operator *(const ImplMatrix &otherMatrix) const
    {
        ImplMatrix m(0.0);
        m += (*this);
        m *= otherMatrix;
        return m;
    }

    /*!
     * \brief Operator *=
     * \param factor
     * \return
     */
    CMatrixBase &operator *=(qreal factor)
    {
        this->m_matrix *= factor;
        return (*this);
    }

    /*!
     * \brief Operator *
     * \param factor
     * \return
     */
    CMatrixBase &operator *(qreal factor)
    {
        ImplMatrix m(0.0);
        m += (*this);
        m *= factor;
        return m;
    }

    /*!
     * \brief Operator /=
     * \param factor
     * \return
     */
    CMatrixBase &operator /=(qreal factor)
    {
        this->m_matrix /= factor;
        return (*this);
    }

    /*!
     * \brief Operator /
     * \param factor
     * \return
     */
    CMatrixBase &operator /(qreal factor)
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
     * \brief Transposed matrix
     * \return
     */
    ImplMatrix transposed() const
    {
        ImplMatrix m(0.0);
        m.m_matrix = this->m_matrix.transposed();
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
     * \brief Set a dedicated value
     * \param value
     */
    void fill(qreal value) { this->m_matrix.fill(value); }

    /*!
     * \brief Get element
     * \param row
     * \return
     */
    double getElement(size_t row, size_t column) const;
};

} // namespace

} // namespace

#endif // guard
