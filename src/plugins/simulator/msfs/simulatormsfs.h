/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_MSFS_SIMULATORMSFS_H
#define BLACKSIMPLUGIN_MSFS_SIMULATORMSFS_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace BlackSimPlugin
{
    namespace Msfs
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
                          BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                          BlackMisc::Network::IClientProvider *clientProvider,
                          QObject *parent = nullptr);

            //! \name ISimulator implementations
            //! @{
            virtual bool connectTo() override;
            //! @}
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
} // ns

#endif // guard
