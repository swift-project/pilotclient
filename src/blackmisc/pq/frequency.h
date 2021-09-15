/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_FREQUENCY_H
#define BLACKMISC_PQ_FREQUENCY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Physical unit frequency
     */
    class BLACKMISC_EXPORT CFrequency : public CPhysicalQuantity<CFrequencyUnit, CFrequency>
    {
    public:
        //! Default constructor
        CFrequency() : CPhysicalQuantity(0, CFrequencyUnit::defaultUnit()) {}

        //! Init by double value
        CFrequency(double value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! Init by int value converted to double
        CFrequency(int value, const CFrequencyUnit &unit) : CFrequency(double(value), unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CFrequency(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CFrequency)

#endif // guard
