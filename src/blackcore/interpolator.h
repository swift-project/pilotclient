/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_INTERPOLATOR_H
#define BLACKCORE_INTERPOLATOR_H

#include "blackmisc/avaircraftsituation.h"
#include <QDateTime>

namespace BlackCore
{
    //! Interpolator, calculation inbetween positions
    class IInterpolator
    {
    public:
        //! Default constructor
        IInterpolator() {}

        //! Virtual destructor
        virtual ~IInterpolator() {}

        //! Init object
        virtual void initialize() = 0;

        //! Add new aircraft situation
        virtual void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        //! Do we have enough situations to start calculating?
        virtual bool hasEnoughAircraftSituations() const = 0;

        //! Get current aircraft situation
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentSituation() = 0;

        //! Get timestamp of the last received aircraft situation
        virtual QDateTime getTimeOfLastReceivedSituation() const = 0;
    };

} // namespace

#endif // guard
