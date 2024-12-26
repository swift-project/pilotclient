// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H
#define SWIFT_MISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H

#include "misc/provider.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Value object encapsulating a list of aircraft models
    class SWIFT_MISC_EXPORT IAircraftModelSetProvider : public IProvider
    {
        virtual void anchor();

    public:
        //! Get the model set models
        virtual CAircraftModelList getModelSet() const = 0;

        //! Get the model set models count
        virtual int getModelSetCount() const = 0;

        //! Constructor
        IAircraftModelSetProvider() = default;
    };

} // namespace swift::misc::simulation

Q_DECLARE_INTERFACE(swift::misc::simulation::IAircraftModelSetProvider,
                    "org.swift-project.misc.simulation.iaircraftmodelsetprovider")

#endif // SWIFT_MISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H
