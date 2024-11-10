// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_TEMPERATURE_H
#define SWIFT_MISC_PQ_TEMPERATURE_H

#include "misc/swiftmiscexport.h"
#include "misc/pq/physicalquantity.h"

namespace swift::misc::physical_quantities
{

    /*!
     * Physical unit temperature
     */
    class SWIFT_MISC_EXPORT CTemperature : public CPhysicalQuantity<CTemperatureUnit, CTemperature>
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

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CTemperature)

#endif // guard
