// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/aircraftmodelsetprovider.h"

namespace swift::misc::simulation
{
    // pin vtables to this file
    void CAircraftModelSetAware::anchor() {}

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

} // namespace swift::misc::simulation
