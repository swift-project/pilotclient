// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSNETWORKCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "core/vatsim/vatsimsettings.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsNetworkComponent;
}
namespace BlackGui::Components
{
    //! General settings for network
    class BLACKGUI_EXPORT CSettingsNetworkComponent : public QFrame
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
} // ns

#endif // guard
