/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELSETPROVIDER_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/provider.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    //! Value object encapsulating a list of aircraft models
    class BLACKMISC_EXPORT IAircraftModelSetProvider : public IProvider
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
    class BLACKMISC_EXPORT CAircraftModelSetAware : public IProviderAware<IAircraftModelSetProvider>
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

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IAircraftModelSetProvider, "org.swift-project.blackmisc.simulation.iaircraftmodelsetprovider")

#endif //guard
