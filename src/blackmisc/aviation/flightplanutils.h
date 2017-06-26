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
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Flight plan utils
        class BLACKMISC_EXPORT CFlightPlanUtils
        {
        public:
            //! Useful value in flight plan remarks
            struct AirlineRemarks
            {
                QString radioTelephony; //!< radio telephony designator
                QString flightOperator; //!< operator, i.e. normally the airline name

                //! Any remarks available
                bool hasAnyRemarks() const
                {
                    return !radioTelephony.isEmpty() || !flightOperator.isEmpty();
                }
            };

            //! Constructor
            CFlightPlanUtils() = delete;

            //! Parse remarks from a flight plan
            static AirlineRemarks parseFlightPlanAirlineRemarks(const QString &remarks);

        private:
            //! Cut the remarks part
            static QString cut(const QString &remarks, const QString &marker);
        };
    } // namespace
} // namespace

#endif // guard
