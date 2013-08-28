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
template <class ImplVector> QString CVector3DBase<ImplVector>::convertToQString(bool /* i18n */) const
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
 * Element (return by reference)
 */
template <class ImplVector> double &CVector3DBase<ImplVector>::getElement(size_t row)
{
    switch (row)
    {
    case 0:
        return this->m_i;
    case 1:
        return this->m_j;
    case 2:
        return this->m_k;
    default:
        Q_ASSERT_X(true, "getElement", "Detected invalid index in 3D vector");
        throw std::range_error("Detected invalid index in 3D vector");
    }
}

/*
 * Element
 */
template <class ImplVector> double CVector3DBase<ImplVector>::getElement(size_t row) const
{
    return const_cast<CVector3DBase<ImplVector>*>(this)->getElement(row);
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
template <class ImplVector> ImplVector CVector3DBase<ImplVector>::crossProduct(const ImplVector &other) const
{
    ImplVector v(other);
    v.m_i = this->m_j * other.m_k - this->m_k * other.m_j;
    v.m_j = this->m_k * other.m_i - this->m_i * other.m_k;
    v.m_k = this->m_i * other.m_j - this->m_j * other.m_i;
    return v;
}

/*
 * Cross product
 */
template <class ImplVector> double CVector3DBase<ImplVector>::dotProduct(const ImplVector &other) const
{
    return this->m_i * other.m_i + this->m_j * other.m_j + this->m_k * other.m_k;
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
template <class ImplVector> void CVector3DBase<ImplVector>::marshallToDbus(QDBusArgument &argument) const
{
    argument << this->m_i;
    argument << this->m_j;
    argument << this->m_k;
}

/*!
 * \brief Stream from DBus
 * \param argument
 */
template <class ImplVector> void CVector3DBase<ImplVector>::unmarshallFromDbus(const QDBusArgument &argument)
{
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
