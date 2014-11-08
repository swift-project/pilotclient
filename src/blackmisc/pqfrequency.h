/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQFREQUENCY_H
#define BLACKMISC_PQFREQUENCY_H

#include "pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*!
         * Physical unit frequency
         */
        class CFrequency : public CValueObjectStdTuple<CFrequency, CPhysicalQuantity<CFrequencyUnit, CFrequency>>
        {
        public:
            //! Default constructor
            CFrequency() : CValueObjectStdTuple(0, CFrequencyUnit::defaultUnit()) {}

            //! Init by double value
            CFrequency(double value, const CFrequencyUnit &unit) : CValueObjectStdTuple(value, unit) {}

            //! Init by int value converted to double
            CFrequency(int value, const CFrequencyUnit &unit) : CFrequency(double(value), unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CFrequency(const QString &unitString) : CValueObjectStdTuple(unitString) {}
        };
    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CFrequency)

#endif // BLACKMISC_PQFREQUENCY_H
