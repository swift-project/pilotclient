// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDFACTORY_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDFACTORY_H

#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

#include <QObject>
#include <QtPlugin>

namespace BlackSimPlugin::Emulated
{
    //! Factory implementation to create CSimulatorEmulated instances
    class CSimulatorEmulatedFactory :
        public QObject,
        public BlackCore::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatoremulated.json")
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
