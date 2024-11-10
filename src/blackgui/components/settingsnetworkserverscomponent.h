// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/blackguiexport.h"
#include "misc/network/settings/serversettings.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsNetworkServersComponent;
}
namespace BlackGui::Components
{
    //! Settings for network servers
    class BLACKGUI_EXPORT CSettingsNetworkServersComponent : public QFrame
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
