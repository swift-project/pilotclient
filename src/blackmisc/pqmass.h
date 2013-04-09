/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PQMASS_H
#define PQMASS_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Mass
 * \author KWB
 */
class CMass : public CPhysicalQuantity<CMassUnit, CMass>
{
public:
    /*!
     * \brief Default constructor
     */
    CMass() : CPhysicalQuantity(0, CMassUnit::kg(), CMassUnit::kg()) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CMass(qint32 value, const CMassUnit &unit) : CPhysicalQuantity(value, unit, CMassUnit::kg()) {}
    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CMass(double value, const CMassUnit &unit) : CPhysicalQuantity(value, unit, CMassUnit::kg()) {}
    /*!
     * \brief Copy constructor
     * \param mass
     */
    CMass(const CPhysicalQuantity &mass) : CPhysicalQuantity(mass) {}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CMass() {}
};
} // namespace
} // namespace

#endif // PQMASS_H
