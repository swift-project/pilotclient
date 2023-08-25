// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_LOGINADVCOMPONENT_H
#define BLACKGUI_COMPONENTS_LOGINADVCOMPONENT_H

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

namespace Ui
{
    class CLoginAdvComponent;
}
namespace BlackMisc::Simulation
{
    class CAircraftModel;
    class CSimulatedAircraft;
}
namespace BlackGui::Components
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
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CLoginAdvComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginAdvComponent() override;

        //! Automatically popup
        void setAutoPopupWizad(bool autoPopup);

        //! Set auto logoff
        void setAutoLogoff(bool autoLogoff);

        //! Login requested
        void toggleNetworkConnection();

        //! Reset state
        void resetState();

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

        //! Logoff due to insufficient simulator frame rate
        void autoLogoffFrameRate(bool fatal);

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
        BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
    };
} // namespace

#endif // guard
