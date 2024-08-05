// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSX_SIMULATORFSX_H
#define BLACKSIMPLUGIN_FSX_SIMULATORFSX_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace BlackSimPlugin::Fsx
{
    //! FSX simulator implementation
    class CSimulatorFsx : public BlackSimPlugin::FsxCommon::CSimulatorFsxCommon
    {
        Q_OBJECT

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorFsx(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                      BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                      BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      BlackMisc::Network::IClientProvider *clientProvider,
                      QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}
    };

    //! Listener for FSX
    class CSimulatorFsxListener : public FsxCommon::CSimulatorFsxCommonListener
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
