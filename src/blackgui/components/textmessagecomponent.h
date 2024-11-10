// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H

#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/network/textmessagelist.h"
#include "misc/aviation/atcstation.h"
#include "misc/audio/audiosettings.h"
#include "misc/identifier.h"
#include "misc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CTextMessageComponent;
}
namespace BlackGui
{
    class CDockWidgetInfoArea;
    class CTextMessageTextEdit;

    namespace Components
    {
        //! Text message widget
        class BLACKGUI_EXPORT CTextMessageComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CTextMessageComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CTextMessageComponent() override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget) override;

            //! Used to allow direct input from global command line when visible
            //! \remark takes the messages, turns it into a ".msg" command, and emits it
            bool handleGlobalCommandLineText(const QString &commandLine, const swift::misc::CIdentifier &originator);

            //! @{
            //! Font size
            void fontSizeMinus();
            void fontSizePlus();
            //! @}

            //! Display the tab for given callsign
            void showCorrespondingTab(const swift::misc::aviation::CCallsign &callsign);

            //! Display the tab for given frequency
            void showCorrespondingTabForFrequency(const swift::misc::physical_quantities::CFrequency &frequency);

            //! Set tab
            void setTab(TextMessageTab tab);

            //! Has an all tab
            bool hasAllMessagesTab() const;

            //! Show the settings
            void showSettings(bool show);

            //! Show an text entry field
            void showTextMessageEntry(bool show);

            //! Focus the text entry field
            void focusTextEntry();

            //! Remove the all tab, the operation cannot be undone
            void removeAllMessagesTab();

            //! Showing real names
            bool isShowingRealNames() const { return m_showRealNames; }

            //! Showing real names
            void setShowingRealNames(bool show) { m_showRealNames = show; }

            // ---------- overlay test messages -------------

            //! Used as overlay and not dock widget
            void setAsUsedInOverlayMode() { m_usedAsOverlayWidget = true; }

            //! Ignore incoming send/receive signals
            void activate(bool send, bool receive);

            //! Text activated
            bool isActivated() const { return m_activeSend || m_activeReceive; }

            //! Rows/columns
            void setAtcButtonsRowsColumns(int rows, int cols, bool setMaxElements);

            //! Background updates or explicitly called
            void setAtcButtonsBackgroundUpdates(bool backgroundUpdates);

            //! Update buttons
            void updateAtcStationsButtons();

        signals:
            //! Message to be displayed in central info window
            //! \remark do not emit in overlay messages
            void displayInInfoWindow(const swift::misc::CVariant &message, int displayDurationMs);

            //! Command line was entered
            void commandEntered(const QString &commandLine, const swift::misc::CIdentifier &originator);

            //! Text message tab selected
            void textMessageTabSelected();

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;
            swift::misc::CIdentifier m_identifier { "TextMessageComponent", this };
            swift::misc::CSetting<Settings::TextMessageSettings> m_messageSettings { this, &CTextMessageComponent::onSettingsChanged };
            swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings { this };
            bool m_usedAsOverlayWidget = false; //!< disables dockwidget parts if used as overlay widget
            bool m_activeSend = true; //!< ignore sent messages
            bool m_activeReceive = true; //!< ignore received messages
            bool m_showRealNames = true;

            //! Enum to widget
            QWidget *getTabWidget(TextMessageTab tab) const;

            //! Related text edit
            CTextMessageTextEdit *getTextEdit(TextMessageTab tab) const;

            //! Select given tab
            void selectTabWidget(TextMessageTab tab);

            //! Select tab by callsign (for private messages)
            void selectTabWidget(const swift::misc::aviation::CCallsign &callsign, bool addIfNotExisting);

            //! Is that a closeable tab (one the user can close)
            bool isCloseableTab(const QWidget *tabWidget) const;

            //! New message tab for given callsign
            QWidget *addNewTextMessageTab(const swift::misc::aviation::CCallsign &callsign);

            //! Find text message tab by callsign
            QWidget *findTextMessageTabByCallsign(const swift::misc::aviation::CCallsign &callsign, bool callsignResolution = false) const;

            //! Find text message tab by its name
            QWidget *findTextMessageTabByName(const QString &name) const;

            //! Private channel text message
            void addPrivateChannelTextMessage(const swift::misc::network::CTextMessage &textMessage);

            //! Own aircraft
            swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const;

            //! For this text message's recepient, is the current tab selected?
            bool isCorrespondingTextMessageTabSelected(const swift::misc::network::CTextMessage &textMessage) const;

            //! Network connected?
            bool isNetworkConnected() const;

            //! @{
            //! Show current frequencies
            void showCurrentFrequenciesFromCockpit();
            void showCurrentFrequenciesFromCockpit(const swift::misc::simulation::CSimulatedAircraft &ownAircraft);
            //! @}

            //! Append text messages (received, to be sent) to GUI
            void displayTextMessage(const swift::misc::network::CTextMessageList &messages);

            //! \copydoc IContextOwnAircraft::parseCommandLine
            QString textMessageToCommand(const QString &enteredLine);

            //! Identifier
            const swift::misc::CIdentifier &componentIdentifier() const { return m_identifier; }

            //! Handle a text message entered
            void handleEnteredTextMessage(const QString &textMessage);

            //! Text messages received
            void onTextMessageReceived(const swift::misc::network::CTextMessageList &messages);

            //! Text messages sent
            void onTextMessageSent(const swift::misc::network::CTextMessage &sentMessage);

            //! Cockpit values changed, used to updated some components
            void onChangedAircraftCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator);

            //! Settings have been checked (group box visible/invisible)
            void onSettingsChecked(bool checked);

            //! Message group box has been checked (group box visible/invisible)
            void onMessageToChecked(bool checked);

            //! Settings have been changed
            void onSettingsChanged();

            //! Latest 1st checked
            void onLatestFirstChanged(bool checked);

            //! Style sheet has been changed
            void onStyleSheetChanged();

            //! ATC Button
            void onAtcButtonClicked(const swift::misc::aviation::CAtcStation &station);

            //! Update settings
            void updateSettings();

            //! Get all CTextMessageTextEdit child objects
            QList<CTextMessageTextEdit *> findAllTextEdit() const;

            //! Global or settings stylesheet
            QString getStyleSheet() const;

            //! Close text message tab
            void closeTextMessageTab();

            //! Update all tabs
            void updateAllTabs();

            //! Top level was changed (used to enable elements when floating)
            void topLevelChanged(QWidget *widget, bool topLevel);

            //! Command line entered
            void textMessageEntered();

            //! Set the real name widget tooltip
            void setTabWidgetDescription(const swift::misc::aviation::CCallsign &callsign, int widgetIndex);

            //! Callsign and relanme if possible
            QString getCallsignAndRealName(const swift::misc::aviation::CCallsign &callsign) const;

            //! Visible widget hack
            bool isVisibleWidgetHack() const;

            //! Callsign from prpoerty of given tab
            swift::misc::aviation::CCallsign getCallsignPropertyForTab(int tabIndex, bool validated) const;

            //! Emit the display in info window signal
            void emitDisplayInInfoWindow(const swift::misc::CVariant &message, int displayDurationMs);

            //! Get the 1st part of the tab text, "DAMBZ: Joe Doe" -> "DAMBZ", "123.45 Foo" -> "123.45"
            static QString firstPartOfTabText(const QString &tabText);
        };
    } // ns
} // ns

#endif // guard
