// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSETTINGSCOMPONENT_H
#define SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSETTINGSCOMPONENT_H

#include <QScopedPointer>

#include "gui/overlaymessagesframe.h"
#include "misc/settingscache.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatorinfo.h"
#include "plugins/simulator/fsxcommon/fsxcommonexport.h"

namespace Ui
{
    class CSimConnectSettingsComponent;
}
namespace swift::simplugin::fsxcommon
{
    /*!
     * A component that gathers all SimConnect related settings.
     */
    class FSXCOMMON_EXPORT CSimConnectSettingsComponent : public swift::gui::COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSimConnectSettingsComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSimConnectSettingsComponent();

    private:
        //! Set the simconnect info
        void setSimConnectInfo();

        //! The the value in the combobox
        void setComboBox(const QString &value);

        //! The P3D version has been changed
        void onP3DVersionChanged(const QString &version);

        swift::misc::simulation::CSimulatorInfo m_simulator { swift::misc::simulation::CSimulatorInfo::FSX };
        swift::misc::CSetting<swift::misc::simulation::settings::TP3DVersion> m_p3dVersion { this };
        QScopedPointer<Ui::CSimConnectSettingsComponent> ui;
        bool m_p3d64bit = false;
    };
} // namespace swift::simplugin::fsxcommon

#endif // SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSETTINGSCOMPONENT_H
