// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "core/vatsim/vatsimsettings.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CSettingsNetworkComponent;
}
namespace swift::gui::components
{
    //! General settings for network
    class SWIFT_GUI_EXPORT CSettingsNetworkComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsNetworkComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsNetworkComponent();

    private:
        QScopedPointer<Ui::CSettingsNetworkComponent> ui;
    };
} // namespace swift::gui::components

#endif // guard
