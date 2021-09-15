/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H
#define BLACKGUI_COMPONENTS_NETWORKDETAILSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "blackcore/data/networksetup.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/loginmode.h"

namespace Ui { class CNetworkDetailsComponent; }
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

        //! Pages
        enum Page
        {
            PageServer,
            PageDetails
        };

        //! Details
        enum Details
        {
            DetailsServer,
            DetailsVoice,
            DetailsBack
        };

        //! Ctor
        explicit CNetworkDetailsComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CNetworkDetailsComponent() override;

        //! FSD setup
        BlackMisc::Network::CFsdSetup getFsdSetup() const;

        //! Allow override even in read only mode
        void setAlwaysAllowOverride(bool allow);

        //! Specific setup enabled?
        bool isFsdSetupOverrideEnabled() const;

        //! Voice setup
        BlackMisc::Audio::CVoiceSetup getVoiceSetup() const;

        //! Specific setup enabled?
        bool isVoiceSetupOverrideEnabled() const;

        //! Login mode
        BlackMisc::Network::CLoginMode getLoginMode() const;

        //! Login mode
        void setLoginMode(BlackMisc::Network::CLoginMode mode);

        //! Reset state
        void resetState();

        //! Selected server
        //! @{
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

        //! Tab index changed
        void onDetailsTabChanged(int index);

        //! Override credentials
        void onOverrideCredentialsToPilot();

        //! VATSIM data file was loaded
        void onWebServiceDataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Change page
        void onChangePage();

        //! Set the back tab
        void setBackTabName();

        BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
        bool m_updatePilotOnServerChanges = true;
        QScopedPointer<Ui::CNetworkDetailsComponent> ui;
    };
} // ns

#endif // guard
