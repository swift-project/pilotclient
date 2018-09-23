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

#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
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
            //! Tabs
            enum Tab
            {
                TextMessagesAll,
                TextMessagesUnicom,
                TextMessagesCom1,
                TextMessagesCom2
            };

            //! Constructor
            explicit CTextMessageComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CTextMessageComponent() override;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget) override;

            //! Text messages received
            void onTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);

            //! Text messages sent
            void onTextMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

            //! Used to allow direct input from global command line when visible
            //! \remark takes the messages, turns it into a ".msg" command, and emits it
            bool handleGlobalCommandLineText(const QString &commandLine, const BlackMisc::CIdentifier &originator);

            //! Display the tab for given callsign
            void showCorrespondingTab(const BlackMisc::Aviation::CCallsign &callsign);

        signals:
            //! Message to be displayed in info window
            void displayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs) const;

            //! Command line was entered
            void commandEntered(const QString commandLine, const BlackMisc::CIdentifier &originator);

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;
            BlackMisc::CIdentifier m_identifier { "TextMessageComponent", this };
            BlackMisc::CSetting<Settings::TextMessageSettings> m_messageSettings { this, &CTextMessageComponent::onSettingsChanged };

            //! Enum to widget
            QWidget *getTabWidget(Tab tab) const;

            //! Related text edit
            CTextMessageTextEdit *getTextEdit(Tab tab) const;

            //! Select given tab
            void selectTabWidget(Tab tab);

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
            bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

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

            //! Cockpit values changed, used to updated some components
            void onChangedAircraftCockpit();

            //! Settings have been checked (group box visible/invisible)
            void onSettingsChecked(bool checked);

            //! Settings have been changed
            void onSettingsChanged();

            //! Style sheet has been changed
            void onStyleSheetChanged();

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
        };
    } // ns
} // ns
#endif // guard
