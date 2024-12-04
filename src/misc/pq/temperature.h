// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_TEMPERATURE_H
#define SWIFT_MISC_PQ_TEMPERATURE_H

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

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

} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CTemperature)

#endif // SWIFT_MISC_PQ_TEMPERATURE_H
