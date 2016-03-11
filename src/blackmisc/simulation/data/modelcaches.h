/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_DATA_MODELCACHES
#define BLACKMISC_SIMULATION_DATA_MODELCACHES

#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            //! Trait for model cache
            struct ModelCache : public BlackMisc::CDataTrait<BlackMisc::Simulation::CAircraftModelList>
            {
                //! Default value
                static const BlackMisc::Simulation::CAircraftModelList &defaultValue()
                {
                    static const BlackMisc::Simulation::CAircraftModelList ml;
                    return ml;
                }
            };

            //! Trait for XP model cache
            struct ModelCacheXP : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachexp"; }
            };

            //! Trait for FSX model cache
            struct ModelCacheFsx : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefsx"; }
            };

            //! Trait for FS9 model cache
            struct ModelCacheFs9 : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachefs9"; }
            };

            //! Trait for P3D model cache
            struct ModelCacheP3D : public ModelCache
            {
                //! Key in data cache
                static const char *key() { return "modelcachep3d"; }
            };

        } // ns
    } // ns
} // ns

#endif // guard
