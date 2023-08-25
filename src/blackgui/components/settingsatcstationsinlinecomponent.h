// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSATCSTATIONSINLINECOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSATCSTATIONSINLINECOMPONENT_H

#include "blackgui/settings/atcstationssettings.h"
#include <QFrame>

namespace Ui
{
    class CSettingsAtcStationsInlineComponent;
}
namespace BlackGui::Components
{
    /*!
     * How ATC stations will be displayed
     */
    class CSettingsAtcStationsInlineComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsAtcStationsInlineComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsAtcStationsInlineComponent();

        //! Get the settings
        Settings::CAtcStationsSettings getSettings() const { return m_atcSettings.get(); }

        //! Set count information
        void setCounts(int all, int inRange);

    signals:
        //! Changed value
        void changed();

    private:
        //! Settings have been changed
        void onSettingsChanged();

        //! Change the settings
        void changeSettings();

        QScopedPointer<Ui::CSettingsAtcStationsInlineComponent> ui;
        BlackMisc::CSetting<BlackGui::Settings::TAtcStationsSettings> m_atcSettings { this, &CSettingsAtcStationsInlineComponent::onSettingsChanged };
    };
} // ns
#endif // guard
