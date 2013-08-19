/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQLENGTH_H
#define BLACKMISC_PQLENGTH_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Physical unit length (length)
 * \author KWB
 */
class CLength : public CPhysicalQuantity<CLengthUnit, CLength>
{
public:
    /*!
     * \brief Default constructor
     */
    CLength() : CPhysicalQuantity(0, CLengthUnit::defaultUnit()) {}

    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CLength(double value, const CLengthUnit &unit) : CPhysicalQuantity(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CLength() {}
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLength)

#endif // BLACKMISC_PQLENGTH_H
