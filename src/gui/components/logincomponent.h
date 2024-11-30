// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_LOGINCOMPONENT_H
#define SWIFT_GUI_LOGINCOMPONENT_H

#include <QFrame>
#include <QIcon>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>

#include "core/data/networksetup.h"
#include "core/vatsim/vatsimsettings.h"
#include "gui/overlaymessagesframe.h"
#include "gui/settings/guisettings.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/callsign.h"
#include "misc/datacache.h"
#include "misc/digestsignal.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/entityflags.h"
#include "misc/network/server.h"
#include "misc/network/user.h"
#include "misc/settingscache.h"
#include "misc/simulation/data/lastmodel.h"
#include "misc/simulation/simulatedaircraft.h"

namespace Ui
{
    class CLoginComponent;
}
namespace swift::misc::simulation
{
    class CAircraftModel;
    class CSimulatedAircraft;
} // namespace swift::misc::simulation
namespace swift::gui::components
{
    /*!
     * Login component to flight network
     */
    class SWIFT_GUI_EXPORT CLoginComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! The tabs
        enum Tab
        {
            LoginVATSIM,
            LoginOthers
        };

        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CLoginComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginComponent() override;

        //! Automatically popup
        void setAutoPopupWizad(bool autoPopup);

        //! Main info area changed
        void mainInfoAreaChanged(const QWidget *currentWidget);

        //! Set a logoff time
        void setLogoffCountdown(std::chrono::seconds timeout = LogoffIntervalSeconds);

        //! Login requested
        void toggleNetworkConnection();

    signals:
        //! Login
        void loginOrLogoffSuccessful();

        //! Cancelled
        void loginOrLogoffCancelled();

        //! Request network settings
        void requestNetworkSettings();

        //! Request to be shown
        void requestLoginPage();

        //! Relevant login data changed (digest version)
        void loginDataChangedDigest();

    private:
        // -------------- values from GUI -----------------

        //! User from VATSIM data
        swift::misc::network::CUser getUserFromPilotGuiValues() const;

        //! Update own callsign (own aircraft from what is set in the GUI)
        //! \return changed?
        bool updateOwnAircraftCallsignAndPilotFromGuiValues();

        // -------------- values to GUI -----------------

        //! Update GUI values
        void updateGui();

        //! Set the "login as" values
        void setGuiLoginAsValues(const swift::misc::simulation::CSimulatedAircraft &ownAircraft);

        // -------------- others -----------------

        //! Selected server (VATSIM)
        swift::misc::network::CServer getCurrentVatsimServer() const;

        //! Selected server (others)
        swift::misc::network::CServer getCurrentOtherServer() const;

        //! Current server based on selected tab
        swift::misc::network::CServer getCurrentServer() const;

        //! Get a prefill model
        swift::misc::simulation::CAircraftModel getPrefillModel() const;

        //! Login cancelled
        void loginCancelled();

        //! Logoff countdown
        void logoffCountdown();

        //! Auto-logoff detection
        void autoLogoffDetection();

        //! Logoff due to insufficient simulator frame rate
        void autoLogoffFrameRate(bool fatal);

        //! Simulator model has been changed
        void onSimulatorModelChanged(const swift::misc::simulation::CAircraftModel &model);

        //! Pause/Continue timeout
        void toggleTimeout();

        //! Show / hide elements for UI depending on login/simulator state
        void updateUiConnectState();

        //! Make disconnect button flash briefly to catch the user's attention
        void blinkConnectButton();

        //! Set OK button string
        void setOkButtonString(bool connected);

        //! Logoff countdown
        void startLogoffTimerCountdown();

        //! Is the VATSIM network tab selected?
        bool isVatsimNetworkTabSelected() const;

        //! Load from settings
        void loadRememberedUserData();

        //! Server changed
        void onSelectedServerChanged(const swift::misc::network::CServer &server);

        //! Simulator status changed
        void onSimulatorStatusChanged(int status);

        //! Network status has changed
        void onNetworkStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                    const swift::misc::network::CConnectionStatus &to);

        //! Tab widget (server) changed
        void onServerTabWidgetChanged(int index);

        //! Has contexts?
        bool hasValidContexts() const;

        static constexpr std::chrono::milliseconds OverlayMessageMs { 5000 };
        static constexpr std::chrono::seconds LogoffIntervalSeconds { 20 }; //!< time before logoff

        QScopedPointer<Ui::CLoginComponent> ui;
        swift::misc::CDigestSignal m_changedLoginDataDigestSignal { this, &CLoginComponent::loginDataChangedDigest,
                                                                    std::chrono::milliseconds { 1500 }, 10 };
        bool m_updatePilotOnServerChanges = true;
        bool m_networkConnected = false;
        bool m_simulatorConnected = false;
        const QIcon m_iconPlay { ":/famfamfam/icons/famfamfam/icons/silk/control_play_blue.png" };
        const QIcon m_iconPause { ":/famfamfam/icons/famfamfam/icons/silk/control_pause_blue.png" };
        std::chrono::seconds m_logoffIntervalSeconds = LogoffIntervalSeconds;
        QTimer m_logoffCountdownTimer; //!< timer for logoff countdown

        swift::misc::CData<swift::misc::simulation::data::TLastModel> m_lastAircraftModel {
            this
        }; //!< recently used aircraft model
        swift::core::data::CNetworkSetup m_networkSetup; //!< servers last used
    };
} // namespace swift::gui::components

#endif // guard
