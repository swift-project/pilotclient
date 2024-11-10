// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "gui/swiftguiexport.h"
#include "misc/network/settings/serversettings.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsNetworkServersComponent;
}
namespace swift::gui::components
{
    //! Settings for network servers
    class SWIFT_GUI_EXPORT CSettingsNetworkServersComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsNetworkServersComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsNetworkServersComponent();

    private:
        //! Reload settings
        void reloadSettings();

        //! Network server selected
        void serverSelected(const QModelIndex &index);

        //! Alter traffic server
        void alterTrafficServer();

        QScopedPointer<Ui::CSettingsNetworkServersComponent> ui;
        swift::misc::CSetting<swift::misc::network::settings::TTrafficServers> m_trafficNetworkServers { this, &CSettingsNetworkServersComponent::reloadSettings };
    };
} // ns

#endif // guard
