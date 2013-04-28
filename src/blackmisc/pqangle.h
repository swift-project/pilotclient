/*  Copyright (C) 2013 VATSIM Community / authors
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
     * \brief Copy constructor
     */
    CAngle(const CAngle &angle) : CPhysicalQuantity(angle) {}

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
     * \brief Virtual destructor
     */
    virtual ~CAngle() {}

    /*!
     * \brief Convenience method PI
     * \return
     */
    static double pi()
    {
        return double(M_PI);
    }

    /*!
     * \brief Value as factor of PI (e.g. 0.5PI)
     * \return
     */
    double piFactor() const
    {
        return BlackMisc::Math::CMath::round(this->convertedSiValueToDouble() / M_PI, 6);
    }
};

} // namespace
} // namespace

#endif // BLACKMISC_PQANGLE_H
