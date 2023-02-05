/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H

#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/identifier.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui { class CTextMessageComponent; }
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
            bool handleGlobalCommandLineText(const QString &commandLine, const BlackMisc::CIdentifier &originator);

            //! @{
            //! Font size
            void fontSizeMinus();
            void fontSizePlus();
            //! @}

            //! Display the tab for given callsign
            void showCorrespondingTab(const BlackMisc::Aviation::CCallsign &callsign);

            //! Display the tab for given frequency
            void showCorrespondingTabForFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

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
            void displayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs);

            //! Command line was entered
            void commandEntered(const QString &commandLine, const BlackMisc::CIdentifier &originator);

            //! Text message tab selected
            void textMessageTabSelected();

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;
            BlackMisc::CIdentifier m_identifier { "TextMessageComponent", this };
            BlackMisc::CSetting<Settings::TextMessageSettings> m_messageSettings { this, &CTextMessageComponent::onSettingsChanged };
            BlackMisc::CSetting<BlackMisc::Audio::TSettings>   m_audioSettings   { this };
            bool m_usedAsOverlayWidget = false; //!< disables dockwidget parts if used as overlay widget
            bool m_activeSend          = true;  //!< ignore sent messages
            bool m_activeReceive       = true;  //!< ignore received messages
            bool m_showRealNames       = true;

            //! Enum to widget
            QWidget *getTabWidget(TextMessageTab tab) const;

            //! Related text edit
            CTextMessageTextEdit *getTextEdit(TextMessageTab tab) const;

            //! Select given tab
            void selectTabWidget(TextMessageTab tab);

            //! Select tab by callsign (for private messages)
            void selectTabWidget(const BlackMisc::Aviation::CCallsign &callsign, bool addIfNotExisting);

            //! Is that a closeable tab (one the user can close)
            bool isCloseableTab(const QWidget *tabWidget) const;

            //! New message tab for given callsign
            QWidget *addNewTextMessageTab(const BlackMisc::Aviation::CCallsign &callsign);

            //! Find text message tab by callsign
            QWidget *findTextMessageTabByCallsign(const BlackMisc::Aviation::CCallsign &callsign, bool callsignResolution = false) const;

            //! Find text message tab by its name
            QWidget *findTextMessageTabByName(const QString &name) const;

            //! Private channel text message
            void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage);

            //! Own aircraft
            BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const;

            //! For this text message's recepient, is the current tab selected?
            bool isCorrespondingTextMessageTabSelected(const BlackMisc::Network::CTextMessage &textMessage) const;

            //! Network connected?
            bool isNetworkConnected() const;

            //! @{
            //! Show current frequencies
            void showCurrentFrequenciesFromCockpit();
            void showCurrentFrequenciesFromCockpit(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);
            //! @}

            //! Append text messages (received, to be sent) to GUI
            void displayTextMessage(const BlackMisc::Network::CTextMessageList &messages);

            //! \copydoc IContextOwnAircraft::parseCommandLine
            QString textMessageToCommand(const QString &enteredLine);

            //! Identifier
            const BlackMisc::CIdentifier &componentIdentifier() const { return m_identifier; }

            //! Handle a text message entered
            void handleEnteredTextMessage(const QString &textMessage);

            //! Text messages received
            void onTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);

            //! Text messages sent
            void onTextMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

            //! Cockpit values changed, used to updated some components
            void onChangedAircraftCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

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
            void onAtcButtonClicked(const BlackMisc::Aviation::CAtcStation &station);

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
            void setTabWidgetDescription(const BlackMisc::Aviation::CCallsign &callsign, int widgetIndex);

            //! Callsign and relanme if possible
            QString getCallsignAndRealName(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Visible widget hack
            bool isVisibleWidgetHack() const;

            //! Callsign from prpoerty of given tab
            BlackMisc::Aviation::CCallsign getCallsignPropertyForTab(int tabIndex, bool validated) const;

            //! Emit the display in info window signal
            void emitDisplayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs);

            //! Get the 1st part of the tab text, "DAMBZ: Joe Doe" -> "DAMBZ", "123.45 Foo" -> "123.45"
            static QString firstPartOfTabText(const QString &tabText);
        };
    } // ns
} // ns

#endif // guard
