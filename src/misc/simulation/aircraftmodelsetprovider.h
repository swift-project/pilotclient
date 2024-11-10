// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H
#define SWIFT_MISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H

#include "misc/simulation/aircraftmodellist.h"
#include "misc/provider.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Value object encapsulating a list of aircraft models
    class SWIFT_MISC_EXPORT IAircraftModelSetProvider : public IProvider
    {
    public:
        //! Get the model set models
        virtual CAircraftModelList getModelSet() const = 0;

        //! Get the model set models count
        virtual int getModelSetCount() const = 0;

        //! Constructor
        IAircraftModelSetProvider() = default;
    };

    //! Delegating class which can be directly used to access an \sa IAircraftModelSetProvider instance
    class SWIFT_MISC_EXPORT CAircraftModelSetAware : public IProviderAware<IAircraftModelSetProvider>
    {
        virtual void anchor();

    public:
        //! Set the provider
        void setOwnAircraftProvider(IAircraftModelSetProvider *provider) { this->setProvider(provider); }

        //! \copydoc IAircraftModelSetProvider::getModelSet
        CAircraftModelList getModelSet() const;

        //! \copydoc IAircraftModelSetProvider::getModelSetCount
        int getModelSetCount() const;

    protected:
        //! Constructor
        CAircraftModelSetAware(IAircraftModelSetProvider *modelSetProvider) : IProviderAware(modelSetProvider) { Q_ASSERT(modelSetProvider); }
    };

} // ns

Q_DECLARE_INTERFACE(swift::misc::simulation::IAircraftModelSetProvider, "org.swift-project.misc.simulation.iaircraftmodelsetprovider")

#endif // guard
