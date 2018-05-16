/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_COMMON_SIMULATORPLUGINCOMMON_H
#define BLACKSIMPLUGIN_COMMON_SIMULATORPLUGINCOMMON_H

#include "blackcore/simulatorcommon.h"
#include <QObject>
#include <QPointer>

namespace BlackGui { namespace Components { class CInterpolationLogDisplayDialog; }}
namespace BlackSimPlugin
{
    namespace Common
    {
        //! Common base class for simulator plugins
        class CSimulatorPluginCommon : public BlackCore::CSimulatorCommon
        {
        public:
            //! Destructor
            virtual ~CSimulatorPluginCommon();

        protected:
            //! Constructor
            CSimulatorPluginCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                   BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                   BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                                   BlackMisc::Weather::IWeatherGridProvider       *weatherGridProvider,
                                   BlackMisc::Network::IClientProvider            *clientProvider,
                                   QObject *parent = nullptr);

            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .drv intdisplay interpolation log display
            //! </pre>
            //! @}
            virtual bool parseDetails(const BlackMisc::CSimpleCommandParser &parser) override;

            // interface implementations
            virtual void unload() override;

            //! Register help
            static void registerHelp();

        private:
            //! Show the interpolator display
            void showInterpolationDisplay();

            QPointer<BlackGui::Components::CInterpolationLogDisplayDialog> m_interpolationDisplay; //!< can be owned by main window after setting a parent
        };
    } // namespace
} // namespace

#endif // guard
