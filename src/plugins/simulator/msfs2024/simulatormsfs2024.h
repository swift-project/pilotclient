// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_MSFS_SIMULATORMSFS2024_H
#define SWIFT_SIMPLUGIN_MSFS_SIMULATORMSFS2024_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace swift::simplugin::msfs2024
{
    //! FSX simulator implementation
    class CSimulatorMsFs2024 : public swift::simplugin::fsxcommon::CSimulatorFsxCommon
    {
        Q_OBJECT

    public:
        //! Constructor, parameters as in \sa swift::core::ISimulatorFactory::create
        CSimulatorMsFs2024(const swift::misc::simulation::CSimulatorPluginInfo &info,
                           swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                           swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                           swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}

        virtual void
        setTrueAltitude(swift::misc::aviation::CAircraftSituation &aircraftSituation,
                        const swift::simplugin::fsxcommon::DataDefinitionOwnAircraft &simulatorOwnAircraft) override;
    };

    //! Listener for MSFS2024
    class CSimulatorMsFs2024Listener : public fsxcommon::CSimulatorFsxCommonListener
    {
        Q_OBJECT

    public:
        //! Constructor
        using CSimulatorFsxCommonListener::CSimulatorFsxCommonListener;

    protected:
        virtual void startImpl() override;
    };
} // namespace swift::simplugin::msfs2024

#endif // guard
