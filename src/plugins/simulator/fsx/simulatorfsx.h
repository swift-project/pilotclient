// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSX_SIMULATORFSX_H
#define SWIFT_SIMPLUGIN_FSX_SIMULATORFSX_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace swift::simplugin::fsx
{
    //! FSX simulator implementation
    class CSimulatorFsx : public namespace swift::simplugin::fsxcommon::CSimulatorFsxCommon
    {
        Q_OBJECT

    public:
        //! Constructor, parameters as in \sa swift::core::ISimulatorFactory::create
        CSimulatorFsx(const swift::misc::simulation::CSimulatorPluginInfo &info,
                      swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                      swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

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
} // namespace swift::simplugin::fsx

#endif // SWIFT_SIMPLUGIN_FSX_SIMULATORFSX_H
