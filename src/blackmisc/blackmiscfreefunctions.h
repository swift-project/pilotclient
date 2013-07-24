/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H
#include "avallclasses.h"
#include "pqallquantities.h"
#include "mathallclasses.h"

namespace BlackMisc {

/*!
 * Free functions in PQ
 */
namespace PhysicalQuantities {

/*!
 * \brief Register all metadata for PQs
 */
void registerMetadata();

} // PQ

/*!
 * Free functions in aviation
 */
namespace Aviation {

/*!
 * \brief Register metadata for aviation
 */
void registerMetadata();

} // Aviation

namespace Math {

/*!
 * \brief Register metadata for math (matrices, vectors)
 */
void registerMetadata();

}

/*!
 * \brief Register all relevant metadata in BlackMisc
 */
void registerMetadata();

} // BlackMisc

#endif // BLACKMISC_FREEFUNCTIONS_H
