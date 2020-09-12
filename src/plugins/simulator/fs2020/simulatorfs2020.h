/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FS2020_SIMULATORFS2020_H
#define BLACKSIMPLUGIN_FS2020_SIMULATORFS2020_H

#include "../fsxcommon/simulatorfsxcommon.h"

namespace BlackSimPlugin
{
    namespace Fs2020
    {
        //! FSX simulator implementation
        class CSimulatorFs2020 : public BlackSimPlugin::FsxCommon::CSimulatorFsxCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFs2020(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
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

        //! Listener for FSX
        class CSimulatorFs2020Listener : public FsxCommon::CSimulatorFsxCommonListener
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
