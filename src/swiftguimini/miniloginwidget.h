// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINILOGINWIDGET_H
#define SWIFTGUIMINI_MINILOGINWIDGET_H

#include "gui/components/logincomponent.h"
#include "gui/settings/minisettings.h"
#include "misc/aviation/callsign.h"
#include "misc/network/entityflags.h"
#include "misc/settingscache.h"

class QWidget;
class QLayout;
class QUrl;
class QLabel;
class QNetworkReply;

namespace swift::gui::components
{
    class CServerListSelector;
    class CNetworkDetailsComponent;
    class CSimBriefDownloadDialog;
} // namespace swift::gui::components

//! Derived, isolated variant of CLoginComponent (Mini-GUI only) that keeps the login button
//! enabled even without an active simulator connection.
//! \remarks CLoginComponent itself remains unchanged (also used by swiftguistandard).
//!
//! Additionally, this class restructures the "Connect" tab view: the "Network" and "Pilot's info"
//! groups are (once) detached from the internal layout and can be taken over externally
//! (see CMiniLoginSettingsDialog) into a separate layout. The remaining tab only shows
//! "Own Aircraft" (General + Advanced stacked directly on top of each other instead of as tabs)
//! as well as, while connected, the server/logoff info.
//!
//! A quick-access server selection box (populated from both the VATSIM FSD server list and the
//! configured "Other servers", see CSettingsNetworkServersComponent / network/trafficservers) is
//! inserted directly above "Own Aircraft", so that a server can be chosen without having to open
//! the separate "Login Configuration" dialog. The selection is kept in sync with the (possibly
//! already relocated) CNetworkDetailsComponent (correct tab + "VATSIM"/"Other Servers" selector),
//! so it is actually used when establishing the connection, and the last choice is persisted
//! (Mini-GUI specific setting, independent from swiftguistandard) and restored on the next
//! Mini-GUI start.
class CMiniLoginWidget : public swift::gui::components::CLoginComponent
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniLoginWidget(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniLoginWidget() override;

    //! Removes the "Network" and "Pilot's info" groups from this widget's own view and attaches
    //! them (reparented) to the given target layout. Must only be called once.
    void moveNetworkAndPilotGroupsTo(QLayout *targetLayout);

    //! Refreshes the "Own Aircraft" display with the current own aircraft model.
    //! \remarks Works around CLoginComponent::mainInfoAreaChanged() only passing an empty model
    //! when no simulator is actively reporting a model (which would otherwise clear the
    //! ICAO/callsign/model fields while still updating the frame color).
    void refreshOwnAircraftDisplay();

    //! Whether, based on TMiniRequireFlightPlanForConnect and the current flight plan availability,
    //! a flight plan should be automatically sent once the connection to the network succeeds.
    bool shouldAutoSendFlightPlan() const { return m_requireFlightPlanSetting.get() && m_hasFlightPlan; }

public slots:
    //! A flight plan has become available (or not), see CFlightPlanComponent::flightPlanDataChanged.
    //! Used together with TMiniRequireFlightPlanForConnect to gate the "Connect" button.
    void setFlightPlanAvailable(bool available);

    //! The flight plan's content (callsign, aircraft type) has changed, see
    //! CMiniFlightPlanDialog::flightPlanContentChanged(). Used to compare against "Own Aircraft"
    //! and show a warning below the "Own Aircraft" group in case of a mismatch.
    void setFlightPlanContent(const swift::misc::aviation::CFlightPlan &flightPlan);

signals:
    //! A flight plan has been loaded from SimBrief via the "Own Aircraft" quick-access button.
    //! \remark Intentionally mirrors CFlightPlanComponent::flightPlanDataChanged() /
    //! CMiniFlightPlanDialog::flightPlanContentChanged(), so CMiniGuiWindow can forward the loaded
    //! flight plan to CMiniFlightPlanDialog (single source of truth for the "Flight Plan Management" dialog).
    void simBriefFlightPlanLoaded(const swift::misc::aviation::CFlightPlan &flightPlan);

private:
    //! Forces the enabled state of the login/logoff button, independent of the
    //! simulator connection status.
    void forceEnableLoginButton();

    //! Connects directly to the relevant "Own Aircraft" GUI elements (callsign line edit, ICAO
    //! selectors), so the mismatch warning is also updated for plain GUI edits that do not
    //! involve a simulator model change or an active network connection (see
    //! COwnAircraftComponent::aircraftDataChanged(), which is only emitted from
    //! onSimulatorModelChanged()).
    void connectOwnAircraftChangeSignals();

    //! Converts the internal tab view of COwnAircraftComponent ("General"/"Advanced")
    //! into a view with the sections stacked directly on top of each other (no more tabs).
    void stackOwnAircraftSections();

    //! Hides the simulator model / livery related widgets ("le_SimulatorModel", "lbl_Model",
    //! "lbl_LiverySend", "cb_Livery") within "Own Aircraft", which are not relevant for the
    //! Mini-GUI (no simulator model string / livery send handling there).
    void hideOwnAircraftModelLiveryWidgets();

    //! Moves "lbl_Model" and "le_SimulatorModel" (originally part of the "Advanced" section) into
    //! the "General" section's grid layout, directly below "Type"/"Airline".
    void moveModelWidgetBelowTypeAndAirline();

    //! Inserts a quick-access SimBrief button (plus info rows for Aircraft/Departure/Destination and
    //! Route) directly below "comp_ModelStringCompleter" within "Own Aircraft".
    void insertSimBriefSection();

    //! The SimBrief button (inserted via insertSimBriefSection()) was pressed: opens the SimBrief
    //! download dialog (same as CFlightPlanComponent::loadFromSimBrief()) and downloads the plan.
    void onLoadFromSimBrief();

    //! Response from the SimBrief download triggered by onLoadFromSimBrief().
    void onSimBriefResponse(QNetworkReply *nwReplyPtr);

    //! Updates m_lblSimBriefInfo1/2 with the given flight plan's Aircraft/Departure/Destination/Route.
    void updateSimBriefInfoLabels(const swift::misc::aviation::CFlightPlan &flightPlan);

    //! Inserts the quick-access server selection box directly above the "Own Aircraft" group.
    void insertServerSelector();

    //! Populates the server selector from the VATSIM FSD servers plus the configured servers
    //! (network/trafficservers).
    void reloadServerSelector();

    //! Gives the "Connect"/"Disconnect" button (pb_Ok) a wider, more prominent minimum width.
    void widenConnectButton();

    //! VATSIM data file was loaded (needed to populate the VATSIM FSD servers)
    void onWebServiceDataRead(swift::misc::network::CEntityFlags::Entity entity,
                              swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

    //! Quick-access server selector has changed -> keep CNetworkDetailsComponent (tab + "VATSIM"/
    //! "Other Servers" selector) in sync and persist the choice.
    void onServerSelectionChanged(const swift::misc::network::CServer &server);

    //! "Own Aircraft" data (callsign, aircraft type) has changed -> re-check against the flight plan
    void onOwnAircraftDataChanged();

    //! Inserts the (initially hidden) red warning label directly below the "Own Aircraft" group.
    void insertAircraftMismatchWarning();

    //! Compares callsign and aircraft type between "Own Aircraft" and the flight plan, and shows/hides
    //! m_mismatchWarningLabel accordingly.
    void updateAircraftMismatchWarning();

    bool m_groupsMoved = false; //!< moveNetworkAndPilotGroupsTo() has already been executed
    bool m_hasFlightPlan = false; //!< whether a flight plan is currently available (see setFlightPlanAvailable())
    swift::misc::aviation::CCallsign m_fpCallsign; //!< callsign as currently entered in the flight plan
    QString m_fpAircraftIcao; //!< aircraft ICAO designator as currently entered in the flight plan
    QLabel *m_mismatchWarningLabel = nullptr; //!< red warning shown below "Own Aircraft" on mismatch
    QLabel *m_lblSimBriefInfo1 = nullptr; //!< "Aircraft"/"Departure"/"Destination" line below the SimBrief button
    QLabel *m_lblSimBriefInfo2 = nullptr; //!< "Route" line below the SimBrief button
    swift::gui::components::CSimBriefDownloadDialog *m_simBriefDialog = nullptr; //!< SimBrief download dialog
    swift::gui::components::CServerListSelector *m_serverSelector = nullptr; //!< quick-access server box
    swift::gui::components::CNetworkDetailsComponent *m_networkDetailsComponent =
        nullptr; //!< captured before a possible relocation (moveNetworkAndPilotGroupsTo())
    swift::misc::CSetting<swift::gui::settings::TMiniLastServer> m_lastServerSetting {
        this
    }; //!< last server chosen in the Mini-GUI (independent from swiftguistandard)
    swift::misc::CSetting<swift::gui::settings::TMiniRequireFlightPlanForConnect> m_requireFlightPlanSetting {
        this
    }; //!< whether a flight plan is required before connecting
};

#endif // SWIFTGUIMINI_MINILOGINWIDGET_H