// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSXFACTORY_H
#define BLACKSIMPLUGIN_SIMULATOR_FSXFACTORY_H

#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

#include <QObject>
#include <QtPlugin>

namespace BlackSimPlugin::Fsx
{
    //! Factory implementation to create CSimulatorFsx instances
    class CSimulatorFsxFactory :
        public QObject,
        public BlackCore::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatorfsx.json")
        Q_INTERFACES(BlackCore::ISimulatorFactory)

    public:
        //! \copydoc BlackCore::ISimulatorFactory::create
        virtual BlackCore::ISimulator *create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                              BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                              BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                              BlackMisc::Network::IClientProvider *clientProvider) override;

        //! \copydoc BlackCore::ISimulatorFactory::createListener
        virtual BlackCore::ISimulatorListener *createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) override;
    };
} // namespace

#endif // guard
