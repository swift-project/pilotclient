/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_MASS_H
#define BLACKMISC_PQ_MASS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{

    /*!
     * Mass
     */
    class BLACKMISC_EXPORT CMass : public CPhysicalQuantity<CMassUnit, CMass>
    {
    public:
        //! Default constructor
        CMass() : CPhysicalQuantity(0, CMassUnit::defaultUnit()) {}

        //! Init by double value
        CMass(double value, const CMassUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CMass(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CMass)

#endif // guard
