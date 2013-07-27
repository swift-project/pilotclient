/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathvector3d.h"
#include "blackmisc/mathmatrix3x3.h"
#include "blackmisc/coordinateecef.h"
#include "blackmisc/coordinatened.h"

namespace BlackMisc
{
namespace Math
{

/*
 * Convert to string
 */
template <class ImplVector> QString CVector3DBase<ImplVector>::stringForConverter(bool /** i18n **/) const
{
    QString s = ("{%1, %2, %3}");
    s = s.arg(QString::number(this->m_i, 'f')).
            arg(QString::number(this->m_j, 'f')).
            arg(QString::number(this->m_k, 'f'));
    return s;
}

/*
 * Vector to zero
 */
template <class ImplVector> void CVector3DBase<ImplVector>::setZero()
{
    this->fill(0.0);
}

/*
 * Vector to zero
 */
template <class ImplVector> void CVector3DBase<ImplVector>::fill(double value)
{
    this->m_i = value;
    this->m_j = value;
    this->m_k = value;
}

/*
 * Element
 */
template <class ImplVector> double CVector3DBase<ImplVector>::getElement(size_t row) const
{
    double d;
    switch (row)
    {
    case 0:
        d = this->m_i;
        break;
    case 1:
        d = this->m_j;
        break;
    case 2:
        d = this->m_k;
        break;
    default:
        Q_ASSERT_X(true, "getElement", "Detected invalid index in 3D vector");
        throw std::range_error("Detected invalid index in 3D vector");
        break;
    }
    return d;
}

/*
 * Set given element
 */
template <class ImplVector> void CVector3DBase<ImplVector>::setElement(size_t row, double value)
{
    switch (row)
    {
    case 0:
        this->m_i = value;
        break;
    case 1:
        this->m_j = value;
        break;
    case 2:
        this->m_k = value;
        break;
    default:
        Q_ASSERT_X(true, "setElement", "Detected invalid index in 3D vector");
        throw std::range_error("Detected invalid index in 3D vector");
        break;
    }
}

/*
 * Cross product
 */
template <class ImplVector> ImplVector CVector3DBase<ImplVector>::crossProduct(const ImplVector &otherVector) const
{
    ImplVector v(otherVector);
    v.m_i = this->m_j * otherVector.m_k - this->m_k * otherVector.m_j;
    v.m_j = this->m_k * otherVector.m_i - this->m_i * otherVector.m_k;
    v.m_k = this->m_i * otherVector.m_j - this->m_j * otherVector.m_i;
    return v;
}

/*
 * Cross product
 */
template <class ImplVector> double CVector3DBase<ImplVector>::dotProduct(const ImplVector &otherVector) const
{
    return this->m_i * otherVector.m_i + this->m_j * otherVector.m_j + this->m_k * otherVector.m_k;
}


/*
 * Multiply with matrix
 */
template <class ImplVector> void CVector3DBase<ImplVector>::matrixMultiplication(const CMatrix3x3 &matrix)
{
    CMatrix3x1 m = matrix * (this->toMatrix3x1());
    this->m_i = m(0, 0);
    this->m_j = m(1, 0);
    this->m_k = m(2, 0);
}

/*
 * Convert to matrix
 */
template <class ImplVector> CMatrix3x1 CVector3DBase<ImplVector>::toMatrix3x1() const
{
    return CMatrix3x1(this->m_i, this->m_j, this->m_k);
}


/*!
 * \brief Stream to DBus
 * \param argument
 */
template <class ImplVector> void CVector3DBase<ImplVector>::marshallToDbus(QDBusArgument &argument) const {
    argument << this->m_i;
    argument << this->m_j;
    argument << this->m_k;
}

/*!
 * \brief Stream from DBus
 * \param argument
 */
template <class ImplVector> void CVector3DBase<ImplVector>::unmarshallFromDbus(const QDBusArgument &argument) {
    argument >> this->m_i;
    argument >> this->m_j;
    argument >> this->m_k;
}

/*
 * Register metadata
 */
template <class ImplVector> void CVector3DBase<ImplVector>::registerMetadata()
{
    qRegisterMetaType<ImplVector>(typeid(ImplVector).name());
    qDBusRegisterMetaType<ImplVector>();
}

// see here for the reason of thess forward instantiations
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CVector3DBase<CVector3D>;
template class CVector3DBase<BlackMisc::Geo::CCoordinateEcef>;
template class CVector3DBase<BlackMisc::Geo::CCoordinateNed>;

} // namespace

} // namespace
