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
    //! \brief Interpolator, calculation inbetween positions
    class IInterpolator
    {
    public:
        //! \brief Default constructor
        IInterpolator() {}

        //! \brief Virtual destructor
        virtual ~IInterpolator() {}

        //! \brief Init object
        virtual void initialize() = 0;

        /*!
         * \brief Add new aircraft situation
         * \param situation
         */
        virtual void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        /*!
         * \brief Do we have enough situations to start calculating?
         * \return
         */
        virtual bool hasEnoughAircraftSituations() const = 0;

        /*!
         * \brief Get current aircraft situation
         * \return
         */
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentSituation() = 0;

        /*!
         * \brief Get timestamp of the last received aircraft situation
         * \return
         */
        virtual const QDateTime &getTimeOfLastReceivedSituation() const = 0;
    };

} // namespace BlackCore

#endif // guard
