/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LOGINCOMPONENT_H
#define BLACKGUI_LOGINCOMPONENT_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/data/networksetup.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"

#include <QFrame>
#include <QIcon>
#include <QTimer>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui { class CLoginComponent; }
namespace BlackMisc
{
    namespace Simulation
    {
        class CAircraftModel;
        class CSimulatedAircraft;
    }
}
namespace BlackGui
{
    namespace Components
    {
        class CDbQuickMappingWizard;

        /*!
         * Login component to flight network
         */
        class BLACKGUI_EXPORT CLoginComponent : public COverlayMessagesFrame
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
            void setLogoffCountdown(int timeoutSeconds = -1);

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
            //! GUI aircraft values, formatted
            struct CGuiAircraftValues
            {
                BlackMisc::Aviation::CCallsign         ownCallsign;
                BlackMisc::Aviation::CAircraftIcaoCode ownAircraftIcao;
                BlackMisc::Aviation::CAirlineIcaoCode  ownAirlineIcao;
                QString ownAircraftCombinedType;
                QString ownAircraftSimulatorModel;
            };

            // -------------- values from GUI -----------------

            //! Values from GUI
            CGuiAircraftValues getAircraftValuesFromGui() const;

            //! User from VATSIM data
            BlackMisc::Network::CUser getUserFromPilotGuiValues() const;

            //! Callsign from GUI
            BlackMisc::Aviation::CCallsign getCallsignFromGui() const;

            //! Update own callsign (own aircraft from what is set in the GUI)
            //! \return changed?
            bool updateOwnAircraftCallsignAndPilotFromGuiValues();

            //! Update own ICAO values (own aircraft from what is set in the GUI)
            //! \return changed?
            bool updateOwnAircaftIcaoValuesFromGuiValues();

            // -------------- values to GUI -----------------

            //! Update GUI values
            void updateGui();

            //! Set ICAO values
            //! \return changed values?
            bool setGuiIcaoValues(const BlackMisc::Simulation::CAircraftModel &model, bool onlyIfEmpty);

            //! Set the "login as" values
            void setGuiLoginAsValues(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            // -------------- others -----------------

            //! Selected server (VATSIM)
            BlackMisc::Network::CServer getCurrentVatsimServer() const;

            //! Selected server (others)
            BlackMisc::Network::CServer getCurrentOtherServer() const;

            //! Current server based on selected tab
            BlackMisc::Network::CServer getCurrentServer() const;

            //! Get a prefill model
            BlackMisc::Simulation::CAircraftModel getPrefillModel() const;

            //! Login cancelled
            void loginCancelled();

            //! VATSIM data file was loaded
            void onWebServiceDataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

            //! Validate aircaft
            bool validateAircraftValues();

            //! Aircraft ICAO code has been changed
            void onChangedAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Airline ICAO code has been changed
            void onChangedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

            //! Settings have been changed
            void reloadOtherServersSetup();

            //! Logoff countdown
            void logoffCountdown();

            //! Auto-logoff detection
            void autoLogoffDetection();

            //! Logoff due to insufficient simulator frame rate
            void autoLogoffFrameRate(bool fatal);

            //! Lookup own model
            void lookupOwnAircraftModel();

            //! Simulator model has been changed
            void onSimulatorModelChanged(const BlackMisc::Simulation::CAircraftModel &model);

            //! Launch mapping wizard
            void mappingWizard();

            //! Pause/Continue timeout
            void toggleTimeout();

            //! Show / hide elements for UI depending on login state
            void setUiLoginState(bool connected);

            //! Own model and ICAO data for GUI and own aircraft
            void setOwnModelAndIcaoValues(const BlackMisc::Simulation::CAircraftModel &ownModel = {});

            //! Set OK button string
            void setOkButtonString(bool connected);

            //! Logoff countdown
            void startLogoffTimerCountdown();

            //! Highlight model field according to model data
            void highlightModelField(const BlackMisc::Simulation::CAircraftModel &model = {});

            //! Is the VATSIM network tab selected?
            bool isVatsimNetworkTabSelected() const;

            //! Load from settings
            void loadRememberedUserData();

            //! Copy credentials to pilot
            void overrideCredentialsToPilot();

            //! Server changed
            void onSelectedServerChanged(const BlackMisc::Network::CServer &server);

            //! Simulator status changed
            void onSimulatorStatusChanged(int status);

            //! Network status has changed
            void onNetworkStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! Tab widget (server) changed
            void onServerTabWidgetChanged(int index);

            //! Has contexts?
            bool hasValidContexts() const;

            //! Set the server buttons visible
            void setServerButtonsVisible(bool visible);

            //! Tab index changed
            void onDetailsTabChanged(int index);

            static constexpr int OverlayMessageMs      = 5000;
            static constexpr int LogoffIntervalSeconds = 20; //!< time before logoff

            QScopedPointer<Ui::CLoginComponent> ui;
            QScopedPointer<CDbQuickMappingWizard> m_mappingWizard; //!< mapping wizard
            BlackMisc::CDigestSignal m_changedLoginDataDigestSignal { this, &CLoginComponent::loginDataChangedDigest, 1500, 10 };
            bool m_autoPopupWizard = false; //!< automatically popup wizard if mapping is needed
            bool m_visible         = false; //!< is this component selected?
            bool m_updatePilotOnServerChanges = true;
            const QIcon m_iconPlay  {":/famfamfam/icons/famfamfam/icons/silk/control_play_blue.png"};
            const QIcon m_iconPause {":/famfamfam/icons/famfamfam/icons/silk/control_pause_blue.png"};
            int m_logoffIntervalSeconds = LogoffIntervalSeconds;
            QTimer m_logoffCountdownTimer; //!< timer for logoff countdown

            BlackMisc::CData<BlackMisc::Simulation::Data::TLastModel> m_lastAircraftModel { this }; //!< recently used aircraft model
            BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
        };
    } // namespace
} // namespace

#endif // guard
