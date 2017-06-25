/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_FLIGHTPLANUTILS_H
#define BLACKMISC_AVIATION_FLIGHTPLANUTILS_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Flight plan utils
        class BLACKMISC_EXPORT CFlightPlanUtils
        {
        public:
            //! Constructor
            CFlightPlanUtils() = delete;

            //! Parse remarks from a flight plan
            static void parseFlightPlanRemarks(const QString &remarks);
        };
    } // namespace
} // namespace

#endif // guard
