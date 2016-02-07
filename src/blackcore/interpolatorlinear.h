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

#include "blackcoreexport.h"
#include "interpolator.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/sequence.h"
#include <QDateTime>

namespace BlackCore
{
    //! Linear interpolator, calculation inbetween positions
    class BLACKCORE_EXPORT CInterpolatorLinear : public IInterpolator
    {
    public:
        //! Constructor
        CInterpolatorLinear(BlackMisc::Simulation::IRemoteAircraftProvider *provider, QObject *parent = nullptr) :
            IInterpolator(provider, "CInterpolatorLinear", parent)
        {}

        //! \copydoc IInterpolator::getInterpolatedSituation
        virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc, bool vtolAiracraft, InterpolationStatus &status) const override;

        //! Log category
        static QString getMessageCategory() { return "swift.interpolatorlinear"; }
   };

} // guard

#endif // guard

