// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSSWIFTPLUGIN_H
#define SWIFT_GUI_COMPONENTS_SETTINGSSWIFTPLUGIN_H

#include <QFrame>

#include "gui/swiftguiexport.h"
#include "misc/simulation/settings/swiftpluginsettings.h"

namespace Ui
{
    class CSettingsSwiftPlugin;
}
namespace swift::gui::components
{
    /*!
     * Settings for the swift pseudo driver
     */
    class SWIFT_GUI_EXPORT CSettingsSwiftPlugin : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSettingsSwiftPlugin(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSettingsSwiftPlugin();

        //! Get the plugin settings
        swift::misc::simulation::settings::CSwiftPluginSettings getPluginSettings() const;

    private:
        //! Settings changed
        void onSettingsChanged();

        //! Save
        void save();

        //! Get settings
        swift::misc::simulation::settings::CSwiftPluginSettings getSettings() const;

        QScopedPointer<Ui::CSettingsSwiftPlugin> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TSwiftPlugin> m_settings {
            this, &CSettingsSwiftPlugin::onSettingsChanged
        };
    };
} // namespace swift::gui::components

#endif // guard
