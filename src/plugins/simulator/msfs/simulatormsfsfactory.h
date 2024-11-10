// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_MSFSFACTORY_H
#define BLACKSIMPLUGIN_SIMULATOR_MSFSFACTORY_H

#include "blackcore/simulator.h"
#include "misc/simulation/simulatorplugininfo.h"

#include <QObject>
#include <QtPlugin>

namespace BlackSimPlugin::Msfs
{
    //! Factory implementation to create CSimulatorFsx instances
    class CSimulatorMsFsFactory :
        public QObject,
        public BlackCore::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatormsfs.json")
        Q_INTERFACES(BlackCore::ISimulatorFactory)

    public:
        //! \copydoc BlackCore::ISimulatorFactory::create
        virtual BlackCore::ISimulator *create(const swift::misc::simulation::CSimulatorPluginInfo &info,
                                              swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                                              swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                              swift::misc::network::IClientProvider *clientProvider) override;

        //! \copydoc BlackCore::ISimulatorFactory::createListener
        virtual BlackCore::ISimulatorListener *createListener(const swift::misc::simulation::CSimulatorPluginInfo &info) override;
    };
} // namespace

#endif // guard
