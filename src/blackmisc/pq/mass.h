// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
