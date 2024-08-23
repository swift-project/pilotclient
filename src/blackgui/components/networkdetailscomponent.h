// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H
#define BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "blackcore/data/networksetup.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/loginmode.h"

namespace Ui
{
    class CNetworkDetailsComponent;
}
namespace BlackGui::Components
{
    //! FSD details
    class CNetworkDetailsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! The tabs
        enum Tab
        {
            LoginVATSIM,
            LoginOthers,
        };

        //! Ctor
        explicit CNetworkDetailsComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CNetworkDetailsComponent() override;

        //! Login mode
        BlackMisc::Network::CLoginMode getLoginMode() const;

        //! Login mode
        void setLoginMode(BlackMisc::Network::CLoginMode mode);

        //! @{
        //! Selected server
        bool isVatsimServerSelected() const;
        bool isOtherServerSelected() const;
        //! @}

        //! Selected server (VATSIM)
        BlackMisc::Network::CServer getCurrentVatsimServer() const;

        //! Selected server (others)
        BlackMisc::Network::CServer getCurrentOtherServer() const;

        //! Current server based on selected tab
        BlackMisc::Network::CServer getCurrentServer() const;

        //! Pilot or Co-pilot callsign?
        bool hasPartnerCallsign() const;

        //! Pilot or Co-pilot callsign
        BlackMisc::Aviation::CCallsign getPartnerCallsign() const;

    signals:
        //! Override the pilot
        void overridePilot(const BlackMisc::Network::CUser &user);

        //! Request network settings
        void requestNetworkSettings();

    private:
        //! Settings have been changed
        void reloadOtherServersSetup();

        //! Tab widget (server) changed
        void onServerTabWidgetChanged(int index);

        //! Server changed
        void onSelectedServerChanged(const BlackMisc::Network::CServer &server);

        //! Set the server buttons visible
        void setServerButtonsVisible(bool visible);

        //! Override credentials
        void onOverrideCredentialsToPilot();

        //! VATSIM data file was loaded
        void onWebServiceDataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

        BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
        bool m_updatePilotOnServerChanges = true;
        QScopedPointer<Ui::CNetworkDetailsComponent> ui;
    };
} // ns

#endif // guard
