/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_AIRCRAFTMODELS_H
#define BLACKCORE_DATA_AIRCRAFTMODELS_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/variant.h"

namespace BlackCore
{
    namespace Data
    {
        //! Trait for own simulator models
        struct OwnSimulatorAircraftModels : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
        {
            //! Key in data cache
            static const char *key() { return "simulator/models"; }

            //! Default value
            static const BlackMisc::Simulation::CAircraftModelList &defaultValue()
            {
                static const BlackMisc::Simulation::CAircraftModelList defaultValue;
                return defaultValue;
            }
        };

        //! Trait for vPilot derived models
        struct VPilotAircraftModels : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
        {
            //! Key in data cache
            static const char *key() { return "vpilot/models"; }

            //! Default value
            static const BlackMisc::Simulation::CAircraftModelList &defaultValue()
            {
                static const BlackMisc::Simulation::CAircraftModelList defaultValue;
                return defaultValue;
            }
        };

    } // ns
} // ns

#endif // guard
