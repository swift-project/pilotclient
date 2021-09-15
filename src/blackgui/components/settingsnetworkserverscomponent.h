/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/settings/serversettings.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSettingsNetworkServersComponent; }
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
        BlackMisc::CSetting<BlackMisc::Network::Settings::TTrafficServers> m_trafficNetworkServers { this, &CSettingsNetworkServersComponent::reloadSettings };
    };
} // ns

#endif // guard
