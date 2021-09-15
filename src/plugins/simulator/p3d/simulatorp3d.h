/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_P3D_H
#define BLACKSIMPLUGIN_SIMULATOR_P3D_H

#include "../fsxcommon/simulatorfsxcommon.h"
#include "../fsxcommon/simconnectobject.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/settingscache.h"

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
        Q_INTERFACES(BlackMisc::Simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(BlackMisc::Simulation::IInterpolationSetupProvider)

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorP3D(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                        BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                        BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                        BlackMisc::Weather::IWeatherGridProvider       *weatherGridProvider,
                        BlackMisc::Network::IClientProvider            *clientProvider,
                        QObject *parent = nullptr);

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        //! @}

#ifdef Q_OS_WIN64
        //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::requestElevation
        virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc BlackCore::ISimulator::followAircraft
        virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
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
        BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TP3DVersion> m_p3dVersion { this };
    };

    //! Listener for P3D
    class CSimulatorP3DListener : public FsxCommon::CSimulatorFsxCommonListener
    {
        Q_OBJECT

    public:
        //! Constructor
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67054 - Constructor inheritance with non-default constructible members
        // using CSimulatorFsxCommonListener::CSimulatorFsxCommonListener;
        CSimulatorP3DListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) : FsxCommon::CSimulatorFsxCommonListener(info) {}

    protected:
        virtual void startImpl() override;

    private:
        BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TP3DVersion> m_p3dVersion { this };
    };
} // ns

#endif // guard
