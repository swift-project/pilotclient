// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_FSXFACTORY_H
#define SWIFT_SIMPLUGIN_SIMULATOR_FSXFACTORY_H

#include <QObject>
#include <QtPlugin>

#include "core/simulator.h"
#include "misc/simulation/simulatorplugininfo.h"

namespace swift::simplugin::fsx
{
    //! Factory implementation to create CSimulatorFsx instances
    class CSimulatorFsxFactory : public QObject, public swift::core::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swift_core.simulatorinterface" FILE "simulatorfsx.json")
        Q_INTERFACES(swift::core::ISimulatorFactory)

    public:
        //! \copydoc swift::core::ISimulatorFactory::create
        virtual swift::core::ISimulator *
        create(const swift::misc::simulation::CSimulatorPluginInfo &info,
               swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
               swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
               swift::misc::network::IClientProvider *clientProvider) override;

        //! \copydoc swift::core::ISimulatorFactory::createListener
        virtual swift::core::ISimulatorListener *
        createListener(const swift::misc::simulation::CSimulatorPluginInfo &info) override;
    };
} // namespace swift::simplugin::fsx

#endif // SWIFT_SIMPLUGIN_SIMULATOR_FSXFACTORY_H
