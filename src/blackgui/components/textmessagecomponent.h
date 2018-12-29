/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_COMPONENTS_TEXTMESSAGECOMPONENT_H

#include "textmessagecomponenttab.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/simulation/simulatedaircraft.h"
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

            //! Display the tab for given callsign
            void showCorrespondingTab(const BlackMisc::Aviation::CCallsign &callsign);

            //! Font size @{
            void fontSizeMinus();
            void fontSizePlus();
            //! @}

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

            // ---------- overlay test messages -------------

            //! Used as overlay and not dock widget
            void setAsUsedInOverlayMode() { m_usedAsOverlayWidget = true; }

            //! Rows/columns
            void setAtcButtonsRowsColumns(int rows, int cols, bool setMaxElements);

            //! Background updates or explicitly called
            void setAtcButtonsBackgroundUpdates(bool backgroundUpdates);

            //! Update buttons
            void updateAtcStationsButtons();

        signals:
            //! Message to be displayed in info window
            void displayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs);

            //! Command line was entered
            void commandEntered(const QString &commandLine, const BlackMisc::CIdentifier &originator);

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;
            BlackMisc::CIdentifier m_identifier { "TextMessageComponent", this };
            BlackMisc::CSetting<Settings::TextMessageSettings> m_messageSettings { this, &CTextMessageComponent::onSettingsChanged };
            bool m_usedAsOverlayWidget = false; //!< disables dockwidget parts

            //! Enum to widget
            QWidget *getTabWidget(TextMessageTab tab) const;

            //! Related text edit
            CTextMessageTextEdit *getTextEdit(TextMessageTab tab) const;

            //! Select given tab
            void selectTabWidget(TextMessageTab tab);

            //! Is that a closeable tab (one the user can close)
            bool isCloseableTab(const QWidget *tabWidget) const;

            //! New message tab for given callsign
            QWidget *addNewTextMessageTab(const BlackMisc::Aviation::CCallsign &callsign);

            //! Add new text message tab
            //! \param tabName name of the new tab, usually the channel name
            QWidget *addNewTextMessageTab(const QString &tabName);

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

            //! Show current frequencies
            void showCurrentFrequenciesFromCockpit();

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
            void onChangedAircraftCockpit();

            //! Settings have been checked (group box visible/invisible)
            void onSettingsChecked(bool checked);

            //! Message group box has been checked (group box visible/invisible)
            void onMessageToChecked(bool checked);

            //! Settings have been changed
            void onSettingsChanged();

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

            //! Top level was changed (used to enable elements when floating)
            void topLevelChanged(QWidget *widget, bool topLevel);

            //! Command line entered
            void textMessageEntered();

            //! Visible widget hack
            bool isVisibleWidgetHack() const;
        };
    } // ns
} // ns
#endif // guard
