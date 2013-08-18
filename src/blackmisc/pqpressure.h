/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQPRESSURE_H
#define BLACKMISC_PQPRESSURE_H

#include "pqphysicalquantity.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CPressure : public CPhysicalQuantity<CPressureUnit, CPressure>
{
public:
    /*!
     * \brief Default constructor
     */
    CPressure() : CPhysicalQuantity(0, CPressureUnit::defaultUnit()) {}

    /**
     *\brief Copy constructor from base type
     */
    CPressure(const CPhysicalQuantity &base) : CPhysicalQuantity(base) {}

    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CPressure() {}
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressure)

#endif // BLACKMISC_PQPRESSURE_H
