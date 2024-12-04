// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_COMMON_SIMULATORPLUGINCOMMON_H
#define SWIFT_SIMPLUGIN_COMMON_SIMULATORPLUGINCOMMON_H

#include <QObject>
#include <QPointer>

#include "core/simulator.h"
#include "plugins/simulator/plugincommon/simulatorplugincommonexport.h"

namespace swift::gui::components
{
    class CInterpolationLogDisplayDialog;
}
namespace swift::simplugin::common
{
    //! Common base class for simulator plugins
    class SIMULATORPLUGINCOMMON_EXPORT CSimulatorPluginCommon : public swift::core::ISimulator
    {
        Q_OBJECT
        Q_INTERFACES(swift::core::ISimulator)
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

    public:
        //! Destructor
        virtual ~CSimulatorPluginCommon() override;

        // --------- ISimulator implementations ------------
        virtual void unload() override;
        virtual bool disconnectFrom() override;

    protected:
        //! Constructor
        CSimulatorPluginCommon(const swift::misc::simulation::CSimulatorPluginInfo &info,
                               swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                               swift::misc::simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                               swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! \ingroup swiftdotcommands
        //! <pre>
        //! .drv intdisplay interpolation log display
        //! </pre>
        virtual bool parseDetails(const swift::misc::CSimpleCommandParser &parser) override;

        //! Register help
        static void registerHelp();

    private:
        //! Show the interpolator display
        void showInterpolationDisplay();

        //! Clean up the interpolation log.display if any
        void deleteInterpolationDisplay();

        QPointer<swift::gui::components::CInterpolationLogDisplayDialog>
            m_interpolationDisplayDialog; //!< can be owned by main window after setting a parent
    };
} // namespace swift::simplugin::common

#endif // SWIFT_SIMPLUGIN_COMMON_SIMULATORPLUGINCOMMON_H
