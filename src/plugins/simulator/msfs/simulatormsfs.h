// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_MSFS_SIMULATORMSFS_H
#define BLACKSIMPLUGIN_MSFS_SIMULATORMSFS_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace BlackSimPlugin::Msfs
{
    //! FSX simulator implementation
    class CSimulatorMsFs : public BlackSimPlugin::FsxCommon::CSimulatorFsxCommon
    {
        Q_OBJECT

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorMsFs(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                       BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                       BlackMisc::Network::IClientProvider *clientProvider,
                       QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}

        virtual void setTrueAltitude(BlackMisc::Aviation::CAircraftSituation &aircraftSituation, const BlackSimPlugin::FsxCommon::DataDefinitionOwnAircraft &simulatorOwnAircraft) override;
    };

    //! Listener for MSFS
    class CSimulatorMsFsListener : public FsxCommon::CSimulatorFsxCommonListener
    {
        Q_OBJECT

    public:
        //! Constructor
        using CSimulatorFsxCommonListener::CSimulatorFsxCommonListener;

    protected:
        virtual void startImpl() override;
    };
} // ns

#endif // guard
