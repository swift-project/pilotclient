// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
