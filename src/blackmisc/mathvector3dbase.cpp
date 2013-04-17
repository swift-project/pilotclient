/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathvector3dbase.h"
#include "blackmisc/mathvector3d.h"
#include "blackmisc/coordinateecef.h"
#include "blackmisc/coordinatened.h"

namespace BlackMisc
{
namespace Math
{

/*
 * Convert to string
 */
template <class ImplClass> QString CVector3DBase<ImplClass>::stringForConverter() const
{
    QString s = ("{%1, %2, %3}");
    s = s.arg(this->m_vector.x()).arg(this->m_vector.y()).arg(this->m_vector.z());
    return s;
}

/*
 * Vector to zero
 */
template <class ImplClass> void CVector3DBase<ImplClass>::setZero()
{
    this->fill(0.0);
}

/*
 * Vector to zero
 */
template <class ImplClass> void CVector3DBase<ImplClass>::fill(qreal value)
{
    this->m_vector.setX(value);
    this->m_vector.setY(value);
    this->m_vector.setZ(value);
}

/*
 * Element
 */
template <class ImplClass> qreal CVector3DBase<ImplClass>::getElement(size_t row) const
{
    bool validIndex = (row < 3 && row >= 0);
    Q_ASSERT_X(validIndex, "getElement", "Wrong index");
    if (!validIndex) throw std::range_error("Invalid index vor 3D vector");
    double d;
    switch (row)
    {
    case 0:
        d = this->m_vector.x();
        break;
    case 1:
        d = this->m_vector.y();
        break;
    case 2:
        d = this->m_vector.z();
        break;
    default:
        Q_ASSERT_X(true, "getElement", "Detected invalid index in 3D vector");
        throw std::range_error("Detected invalid index in 3D vector");
        break;
    }
    return d;
}

// see here for the reason of thess forward instantiations
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CVector3DBase<CVector3D>;
template class CVector3DBase<BlackMisc::Geo::CCoordinateEcef>;
template class CVector3DBase<BlackMisc::Geo::CCoordinateNed>;

} // namespace

} // namespace
