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
        //! Linear interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT CInterpolatorLinear : public CInterpolator<CInterpolatorLinear>
        {
            Q_OBJECT

        public:
            //! Constructor
            CInterpolatorLinear(QObject *parent = nullptr) :
                CInterpolator("CInterpolatorLinear", parent)
            {}

            //! \copydoc IInterpolator::getInterpolatedSituation
            BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                const BlackMisc::Simulation::CInterpolationHints &hints, CInterpolationStatus &status) const;

            //! Log category
            static QString getLogCategory() { return "swift.interpolatorlinear"; }
        };
    } // ns
} // ns

#endif // guard
