/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSNETWORKSERVERSCOMPONENT_H

#include "blackcore/settings/network.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QModelIndex;
class QWidget;

namespace Ui { class CSettingsNetworkServersComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Settings for network servers
        class BLACKGUI_EXPORT CSettingsNetworkServersComponent :
            public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsNetworkServersComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsNetworkServersComponent();

        private slots:
            //! Reload settings
            void ps_reloadSettings();

            //! Network server selected
            void ps_serverSelected(const QModelIndex &index);

            //! Alter traffic server
            void ps_alterTrafficServer();

        private:
            QScopedPointer<Ui::CSettingsNetworkServersComponent> ui;
            BlackMisc::CSetting<BlackCore::Settings::Network::TrafficServers> m_trafficNetworkServers { this, &CSettingsNetworkServersComponent::ps_reloadSettings };
        };
    } // ns
} // ns

#endif // guard
