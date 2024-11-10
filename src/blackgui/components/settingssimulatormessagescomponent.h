// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSSIMULATORMESSAGESCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSSIMULATORMESSAGESCOMPONENT_H

#include "misc/simulation/settings/simulatorsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsSimulatorMessagesComponent;
}
namespace BlackGui::Components
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
        virtual ~CSettingsSimulatorMessagesComponent() override;

    private:
        //! Save the data
        void save();

        //! Load data
        void load();

    private:
        QScopedPointer<Ui::CSettingsSimulatorMessagesComponent> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TSimulatorMessages> m_settings { this }; //!< settings for messages
    };
} // ns
#endif // guard
