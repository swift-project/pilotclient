/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LOGINADVCOMPONENT_H
#define BLACKGUI_LOGINADVCOMPONENT_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/data/networksetup.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/callsign.h"
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

namespace Ui { class CLoginAdvComponent; }
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
        class BLACKGUI_EXPORT CLoginAdvComponent : public COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            explicit CLoginAdvComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CLoginAdvComponent() override;

            //! Automatically popup
            void setAutoPopupWizad(bool autoPopup);

            //! Main info area changed
            void mainInfoAreaChanged(const QWidget *currentWidget);

            //! Login requested
            void toggleNetworkConnection();

        signals:
            //! Login
            void loginOrLogoffSuccessful();

            //! Cancelled
            void loginOrLogoffCancelled();

            //! Request to be shown
            void requestLoginPage();

            //! Request server settigs
            void requestNetworkSettings();

            //! Relevant login data changed (digest version)
            void loginDataChangedDigest();

        private:
            // -------------- values from GUI -----------------

            //! User from VATSIM data
            BlackMisc::Network::CUser getUserFromPilotGuiValues() const;

            //! Callsign from GUI
            // BlackMisc::Aviation::CCallsign getCallsignFromGui() const;

            //! Update own callsign (own aircraft from what is set in the GUI)
            //! \return changed?
            bool updateOwnAircraftCallsignAndPilotFromGuiValues();

            // -------------- values to GUI -----------------

            //! Update GUI values
            void updateGui();

            //! Set the "login as" values
            void setGuiLoginAsValues(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            // -------------- others -----------------

            //! Selected server (VATSIM)
            BlackMisc::Network::CServer getCurrentVatsimServer() const;

            //! Selected server (others)
            BlackMisc::Network::CServer getCurrentOtherServer() const;

            //! Current server based on selected tab
            BlackMisc::Network::CServer getCurrentServer() const;

            //! Login cancelled
            void loginCancelled();

            //! Auto-logoff detection
            void autoLogoffDetection();

            //! Pause/Continue timeout
            void toggleTimeout();

            //! Set OK button string
            void setOkButtonString(bool connected);

            //! Load from settings
            void loadRememberedUserData();

            //! Copy credentials to pilot
            void overrideCredentialsToPilot(const BlackMisc::Network::CUser &user);

            //! Simulator status changed
            void onSimulatorStatusChanged(int status);

            //! Has contexts?
            bool hasValidContexts() const;

            static constexpr int OverlayMessageMs = 5000;
            static constexpr int LogoffIntervalSeconds = 20; //!< time before logoff

            QScopedPointer<Ui::CLoginAdvComponent> ui;
            BlackMisc::CDigestSignal m_changedLoginDataDigestSignal { this, &CLoginAdvComponent::loginDataChangedDigest, 1500, 10 };
            bool m_visible         = false; //!< is this component selected?
            BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
        };
    } // namespace
} // namespace

#endif // guard
