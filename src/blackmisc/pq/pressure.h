/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_PRESSURE_H
#define BLACKMISC_PQ_PRESSURE_H

#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<PhysicalQuantities::CPressure> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::DefaultAndQList;
    };

    namespace PhysicalQuantities
    {

        /*!
         * Physical unit distance
         */
        class CPressure : public CValueObject<CPressure, CPhysicalQuantity<CPressureUnit, CPressure>>
        {
        public:
            //! Default constructor
            CPressure() : CValueObject(0, CPressureUnit::defaultUnit()) {}

            //! Init by double value
            CPressure(double value, const CPressureUnit &unit) : CValueObject(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CPressure(const QString &unitString) : CValueObject(unitString) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressure)

#endif // BLACKMISC_PQPRESSURE_H
