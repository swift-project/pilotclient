// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PQ_TEMPERATURE_H
#define BLACKMISC_PQ_TEMPERATURE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{

    /*!
     * Physical unit temperature
     */
    class BLACKMISC_EXPORT CTemperature : public CPhysicalQuantity<CTemperatureUnit, CTemperature>
    {
    public:
        //! Default constructor
        CTemperature() : CPhysicalQuantity(0, CTemperatureUnit::defaultUnit()) {}

        //! Init by double value
        CTemperature(double value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CTemperature(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTemperature)

#endif // guard
