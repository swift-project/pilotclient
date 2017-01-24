/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H
#define BLACKMISC_SIMULATION_INTERPOLATORLINEAR_H

#include "interpolator.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include <QString>
#include <QtGlobal>

class QObject;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation
    {
        class IRemoteAircraftProvider;

        //! Linear interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT CInterpolatorLinear : public IInterpolator
        {
        public:
            //! Constructor
            CInterpolatorLinear(BlackMisc::Simulation::IRemoteAircraftProvider *provider, QObject *parent = nullptr) :
                IInterpolator(provider, "CInterpolatorLinear", parent)
            {}

            // public base class signature
            using IInterpolator::getInterpolatedSituation;

            //! \copydoc IInterpolator::getInterpolatedSituation
            virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CCallsign &callsign,
                const BlackMisc::Aviation::CAircraftSituationList &situations, qint64 currentTimeSinceEpoc,
                const BlackMisc::Simulation::CInterpolationHints &hints, InterpolationStatus &status) const override;

            //! Log category
            static QString getLogCategory() { return "swift.interpolatorlinear"; }
        };
    } // ns
} // ns

#endif // guard
