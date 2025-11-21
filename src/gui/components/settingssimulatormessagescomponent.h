// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSSIMULATORMESSAGESCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSSIMULATORMESSAGESCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/simulation/settings/simulatorsettings.h"

namespace Ui
{
    class CSettingsSimulatorMessagesComponent;
}
namespace swift::gui::components
{
    /*!
     * Configure what messages are sent to simulator
     */
    class CSettingsSimulatorMessagesComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsSimulatorMessagesComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsSimulatorMessagesComponent() override;

    private:
        //! Save the data
        void save();

        //! Load data
        void load();

    private:
        QScopedPointer<Ui::CSettingsSimulatorMessagesComponent> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorMessages> m_settings {
            this
        }; //!< settings for messages
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_SETTINGSSIMULATORMESSAGESCOMPONENT_H
