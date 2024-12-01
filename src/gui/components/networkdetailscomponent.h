// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "core/data/networksetup.h"
#include "misc/datacache.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/entityflags.h"
#include "misc/network/fsdsetup.h"
#include "misc/network/loginmode.h"
#include "misc/settingscache.h"

namespace Ui
{
    class CNetworkDetailsComponent;
}
namespace swift::gui::components
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
        swift::misc::network::CLoginMode getLoginMode() const;

        //! Login mode
        void setLoginMode(swift::misc::network::CLoginMode mode);

        //! @{
        //! Selected server
        bool isVatsimServerSelected() const;
        bool isOtherServerSelected() const;
        //! @}

        //! Selected server (VATSIM)
        swift::misc::network::CServer getCurrentVatsimServer() const;

        //! Selected server (others)
        swift::misc::network::CServer getCurrentOtherServer() const;

        //! Current server based on selected tab
        swift::misc::network::CServer getCurrentServer() const;

        //! Pilot or Co-pilot callsign?
        bool hasPartnerCallsign() const;

        //! Pilot or Co-pilot callsign
        swift::misc::aviation::CCallsign getPartnerCallsign() const;

    signals:
        //! Override the pilot
        void overridePilot(const swift::misc::network::CUser &user);

        //! Request network settings
        void requestNetworkSettings();

    private:
        //! Settings have been changed
        void reloadOtherServersSetup();

        //! Tab widget (server) changed
        void onServerTabWidgetChanged(int index);

        //! Server changed
        void onSelectedServerChanged(const swift::misc::network::CServer &server);

        //! Set the server buttons visible
        void setServerButtonsVisible(bool visible);

        //! VATSIM data file was loaded
        void onWebServiceDataRead(swift::misc::network::CEntityFlags::Entity entity,
                                  swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        swift::core::data::CNetworkSetup m_networkSetup; //!< servers last used
        bool m_updatePilotOnServerChanges = true;
        QScopedPointer<Ui::CNetworkDetailsComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H
