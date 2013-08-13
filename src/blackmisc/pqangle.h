/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQANGLE_H
#define BLACKMISC_PQANGLE_H
#include "blackmisc/pqphysicalquantity.h"
#include "blackmisc/mathematics.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Physical unit angle (radians, degrees)
 */
class CAngle : public CPhysicalQuantity<CAngleUnit, CAngle>
{
public:
    /*!
     * \brief Default constructor
     */
    CAngle() : CPhysicalQuantity(0, CAngleUnit::rad(), CAngleUnit::rad()) {}

    /*!
     * \brief Copy constructor from base type
     */
    CAngle(const CPhysicalQuantity &base) : CPhysicalQuantity(base) {}

    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CAngle(qint32 value, const CAngleUnit &unit): CPhysicalQuantity(value, unit, CAngleUnit::rad()) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CAngle(double value, const CAngleUnit &unit): CPhysicalQuantity(value, unit, CAngleUnit::rad()) {}

    /*!
     * \brief Intir as sexagesimal degrees
     * \param degrees
     * \param minutes
     * \param seconds
     */
    CAngle(qint32 degrees, qint32 minutes, double seconds) :
        CPhysicalQuantity(
            degrees + minutes / 100.0 + seconds / 10000.0,
            CAngleUnit::sexagesimalDeg(), CAngleUnit::rad()) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CAngle() {}

    /*!
     * \brief Value as factor of PI (e.g. 0.5PI)
     * \return
     */
    double piFactor() const
    {
        return BlackMisc::Math::CMath::round(this->convertedSiValueToDouble() / BlackMisc::Math::CMath::PI() , 6);
    }

    /*!
     * \brief PI as convenience method
     * \return
     */
    static const double &PI()
    {
        return BlackMisc::Math::CMath::PI();
    }
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAngle)

#endif // BLACKMISC_PQANGLE_H
