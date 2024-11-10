// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_P3D_H
#define BLACKSIMPLUGIN_SIMULATOR_P3D_H

#include "../fsxcommon/simulatorfsxcommon.h"
#include "../fsxcommon/simconnectobject.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/settingscache.h"

namespace BlackSimPlugin::P3D
{
    //! P3D specific events
    enum EventsIdsP3D
    {
        EventP3dFoo = FsxCommon::EventFSXEndMarker + 1
    };

    //! P3D Simulator Implementation
    class CSimulatorP3D : public FsxCommon::CSimulatorFsxCommon
    {
        Q_OBJECT
        Q_INTERFACES(BlackCore::ISimulator)
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorP3D(const swift::misc::simulation::CSimulatorPluginInfo &info,
                      swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                      swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      swift::misc::network::IClientProvider *clientProvider,
                      QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}

#ifdef Q_OS_WIN64
        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevation
        virtual bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference, const swift::misc::aviation::CCallsign &callsign) override;

        //! \copydoc BlackCore::ISimulator::followAircraft
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;
#endif // guard

    protected:
        //! \copydoc FsxCommon::CSimulatorFsxCommon::initEventsP3D
        virtual HRESULT initEventsP3D() override;

#ifdef Q_OS_WIN64
        //! \copydoc FsxCommon::CSimulatorFsxCommon::removeCamera
        virtual void removeCamera(FsxCommon::CSimConnectObject &simObject) override;

        //! \copydoc FsxCommon::CSimulatorFsxCommon::removeObserver
        virtual void removeObserver(FsxCommon::CSimConnectObject &simObject) override;

        //! \copydoc FsxCommon::CSimulatorFsxCommon::releaseAIControl
        //! \remark P3D API release of control
        virtual bool releaseAIControl(const FsxCommon::CSimConnectObject &simObject, SIMCONNECT_DATA_REQUEST_ID requestId) override;
#endif
        //! SimConnect Callback
        static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

    private:
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TP3DVersion> m_p3dVersion { this };
    };

    //! Listener for P3D
    class CSimulatorP3DListener : public FsxCommon::CSimulatorFsxCommonListener
    {
        Q_OBJECT

    public:
        //! Constructor
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67054 - Constructor inheritance with non-default constructible members
        // using CSimulatorFsxCommonListener::CSimulatorFsxCommonListener;
        CSimulatorP3DListener(const swift::misc::simulation::CSimulatorPluginInfo &info) : FsxCommon::CSimulatorFsxCommonListener(info) {}

    protected:
        virtual void startImpl() override;

    private:
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TP3DVersion> m_p3dVersion { this };
    };
} // ns

#endif // guard
