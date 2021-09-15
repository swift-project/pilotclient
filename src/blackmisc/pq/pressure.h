/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_PRESSURE_H
#define BLACKMISC_PQ_PRESSURE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Physical unit distance
     */
    class BLACKMISC_EXPORT CPressure : public CPhysicalQuantity<CPressureUnit, CPressure>
    {
    public:
        //! Default constructor
        CPressure() : CPhysicalQuantity(0, CPressureUnit::defaultUnit()) {}

        //! Init by double value
        CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CPressure(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressure)

#endif // guard
