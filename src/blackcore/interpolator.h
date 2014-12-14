/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.  */

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
        virtual const QDateTime &getTimeOfLastReceivedSituation() const = 0;
    };

} // namespace BlackCore

#endif // guard
