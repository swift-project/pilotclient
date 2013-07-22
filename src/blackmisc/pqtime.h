/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQTIME_H
#define BLACKMISC_PQTIME_H

#include "pqphysicalquantity.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Time class, e.g. "ms", "hour", "s", "day"
 * \author KWB
 */
class CTime : public CPhysicalQuantity<CTimeUnit, CTime>
{
public:
    /*!
     * \brief Default constructor
     */
    CTime() : CPhysicalQuantity(0, CTimeUnit::s(), CTimeUnit::s()) {}
    /**
     *\brief Copy constructor
     */
    CTime(const CPhysicalQuantity &time): CPhysicalQuantity(time) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CTime(qint32 value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit, CTimeUnit::s()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CTime(double value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit, CTimeUnit::s()) {}
    /*!
     * \brief Destructor
     */
    virtual ~CTime() {}
};
} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTime)

#endif // BLACKMISC_PQTIME_H
