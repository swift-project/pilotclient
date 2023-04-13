/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodelsetprovider.h"

namespace BlackMisc::Simulation
{
    // pin vtables to this file
    void CAircraftModelSetAware::anchor()
    {}

    CAircraftModelList CAircraftModelSetAware::getModelSet() const
    {
        if (!this->hasProvider()) { return CAircraftModelList(); }
        return this->provider()->getModelSet();
    }

    int CAircraftModelSetAware::getModelSetCount() const
    {
        if (!this->hasProvider()) { return -1; }
        return this->provider()->getModelSetCount();
    }

} // ns
