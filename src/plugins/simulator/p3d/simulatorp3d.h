// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_P3D_H
#define SWIFT_SIMPLUGIN_SIMULATOR_P3D_H

#include "../fsxcommon/simconnectobject.h"
#include "../fsxcommon/simulatorfsxcommon.h"

#include "misc/settingscache.h"
#include "misc/simulation/settings/simulatorsettings.h"

namespace swift::simplugin::p3d
{
    //! P3D specific events
    enum EventsIdsP3D
    {
        EventP3dFoo = fsxcommon::EventFSXEndMarker + 1
    };

    //! P3D Simulator Implementation
    class CSimulatorP3D : public fsxcommon::CSimulatorFsxCommon
    {
        Q_OBJECT
        Q_INTERFACES(swift::core::ISimulator)
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

    public:
        //! Constructor, parameters as in \sa swift::core::ISimulatorFactory::create
        CSimulatorP3D(const swift::misc::simulation::CSimulatorPluginInfo &info,
                      swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                      swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}

#ifdef Q_OS_WIN64
        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevation
        virtual bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference,
                                      const swift::misc::aviation::CCallsign &callsign) override;

        //! \copydoc swift::core::ISimulator::followAircraft
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;
#endif

    protected:
        //! \copydoc fsxcommon::CSimulatorFsxCommon::initEventsP3D
        virtual HRESULT initEventsP3D() override;

#ifdef Q_OS_WIN64
        //! \copydoc fsxcommon::CSimulatorFsxCommon::removeCamera
        virtual void removeCamera(fsxcommon::CSimConnectObject &simObject) override;

        //! \copydoc fsxcommon::CSimulatorFsxCommon::removeObserver
        virtual void removeObserver(fsxcommon::CSimConnectObject &simObject) override;

        //! \copydoc fsxcommon::CSimulatorFsxCommon::releaseAIControl
        //! \remark P3D API release of control
        virtual bool releaseAIControl(const fsxcommon::CSimConnectObject &simObject,
                                      SIMCONNECT_DATA_REQUEST_ID requestId) override;
#endif
        //! SimConnect Callback
        static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

    private:
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TP3DVersion> m_p3dVersion { this };
    };

    //! Listener for P3D
    class CSimulatorP3DListener : public fsxcommon::CSimulatorFsxCommonListener
    {
        Q_OBJECT

    public:
        //! Constructor
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67054 - Constructor inheritance with non-default constructible
        // members using CSimulatorFsxCommonListener::CSimulatorFsxCommonListener;
        CSimulatorP3DListener(const swift::misc::simulation::CSimulatorPluginInfo &info)
            : fsxcommon::CSimulatorFsxCommonListener(info)
        {}

    protected:
        virtual void startImpl() override;

    private:
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TP3DVersion> m_p3dVersion { this };
    };
} // namespace swift::simplugin::p3d

#endif // SWIFT_SIMPLUGIN_SIMULATOR_P3D_H
