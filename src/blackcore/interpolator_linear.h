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
    //! Linear interpolator, calculation inbetween positions
    class CInterpolatorLinear : public IInterpolator
    {
    public:
        //! Constructor
        CInterpolatorLinear(BlackMisc::Simulation::IRemoteAircraftProviderReadOnly *provider, QObject *parent = nullptr) :
            IInterpolator(provider, "CInterpolatorLinear", parent)
        {}

        //! \copydoc IInterpolator::getCurrentInterpolatedSituation
        virtual BlackMisc::Aviation::CAircraftSituation getCurrentInterpolatedSituation(const QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> &allSituations, const BlackMisc::Aviation::CCallsign &callsign, bool *ok = nullptr) const override;
    };

} // namespace BlackCore

#endif // guard

