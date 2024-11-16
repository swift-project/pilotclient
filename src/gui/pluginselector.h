// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_PLUGINSELECTOR_H
#define SWIFT_GUI_PLUGINSELECTOR_H

#include <QObject>
#include <QSignalMapper>
#include <QString>
#include <QWidget>

#include "gui/swiftguiexport.h"

namespace swift::gui
{
    /*!
     * Shows all available plugins in a nice list and lets
     * user enable, disable and configure each of them.
     */
    class SWIFT_GUI_EXPORT CPluginSelector : public QWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CPluginSelector(QWidget *parent = nullptr);

        //! Adds the new plugin to the list.
        //! \param identifier Identifier of the plugin
        //! \param name Name of the plugin
        //! \param hasConfig Defines whether the plugin has the corresponding config plugin or not
        //! \param enabled Defines whether the plugin is initially enabled or not
        void addPlugin(const QString &identifier, const QString &name, bool hasConfig = false, bool enabled = true);

        //! Enables/disabled the given plugin.
        void setEnabled(const QString &identifier, bool enabled);

    signals:
        //! Emitted when user enables/disables the particular plugin
        void pluginStateChanged(const QString &identifier, bool enabled);

        //! Emitted when user clicks the "Settings" button
        void pluginConfigRequested(const QString &identifier);

    private:
        void handlePluginStateChange();

        QSignalMapper *m_configButtonMapper = new QSignalMapper(this);
    };
} // namespace swift::gui

#endif // guard
