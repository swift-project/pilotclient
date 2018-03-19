/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORDUMMY_H
#define BLACKMISC_SIMULATION_INTERPOLATORDUMMY_H

#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/interpolator.h"

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Dummy interpolator which does nothing.
         */
        class BLACKMISC_EXPORT CInterpolatorDummy : public QObject
        {
        public:
            //! Constructor
            CInterpolatorDummy(const BlackMisc::Aviation::CCallsign &callsign, QObject *parent) : QObject(parent)
            {
                this->setObjectName("CInterpolatorDummy for " + callsign.asString());
            }

            //! \copydoc CInterpolator::getInterpolatedSituation
            BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                qint64, const CInterpolationAndRenderingSetupGlobal &, const CInterpolationHints &, CInterpolationStatus &) { return {}; }

            //! \copydoc CInterpolator::getInterpolatedParts
            BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                qint64, const CInterpolationAndRenderingSetupGlobal &, CPartsStatus &, bool = false) const { return {}; }

            //! \copydoc CInterpolator::addAircraftSituation
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &) {}

            //! \copydoc CInterpolator::hasAircraftSituations
            bool hasAircraftSituations() const { return false; }

            //! \copydoc CInterpolator::addAircraftParts
            void addAircraftParts(const BlackMisc::Aviation::CAircraftParts &) {}

            //! \copydoc CInterpolator::hasAircraftParts
            bool hasAircraftParts() const { return false; }

            //! \copydoc CInterpolator::getInterpolatorInfo
            QString getInterpolatorInfo() const { return this->objectName(); }

            //! \copydoc CInterpolator::attachLogger
            void attachLogger(CInterpolationLogger *) {}
        };
    } // ns
} // ns

#endif
