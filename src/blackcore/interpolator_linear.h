/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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
        virtual QDateTime getTimeOfLastReceivedSituation() const override;

    private:
         BlackMisc::Aviation::CAircraftSituationList m_aircraftSituationList;

    };

} // namespace BlackCore

#endif // guard

