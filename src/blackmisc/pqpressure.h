/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PQPRESSURE_H
#define PQPRESSURE_H

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
    CPressure() : CPhysicalQuantity(0, CPressureUnit::Pa(), CPressureUnit::Pa()) {}
    /**
     *\brief Copy constructor
     */
    CPressure(const CPressure &pressure) : CPhysicalQuantity(pressure) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CPressure(qint32 value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa()) {}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CPressure() {}
};
} // namespace
} // namespace
#endif // PQPRESSURE_H
