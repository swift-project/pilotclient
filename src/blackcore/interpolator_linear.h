/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.  */

#ifndef BLACKCORE_INTERPOLATOR_LINEAR_H
#define BLACKCORE_INTERPOLATOR_LINEAR_H

#include "interpolator.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/avaircraftsituationlist.h"
#include "blackmisc/sequence.h"
#include <QDateTime>

namespace BlackCore
{
    //! \brief Linear interpolator, calculation inbetween positions
    class CInterpolatorLinear :  public IInterpolator
    {
    public:
        //! \brief Default constructor
        CInterpolatorLinear() {}

        //! \brief Virtual destructor
        virtual ~CInterpolatorLinear() {}

        //! Init object
        virtual void initialize() override;

        //! \copydoc IInterpolator::addSituation()
        virtual void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

        //! \copydoc IInterpolator::hasEnoughAircraftSituations()
        virtual bool hasEnoughAircraftSituations() const override;

        //! \copydoc IInterpolator::getCurrentSituation()
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentSituation() override;

        //! \copydoc IInterpolator::getTimeOfLastReceivedSituation()
        virtual const QDateTime &getTimeOfLastReceivedSituation() const override;

    private:
         BlackMisc::Aviation::CAircraftSituationList m_aircraftSituationList;

    };

} // namespace BlackCore

#endif // guard

