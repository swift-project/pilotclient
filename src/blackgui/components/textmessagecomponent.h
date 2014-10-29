/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_TEXTMESSAGECOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackmisc/nwtextmessage.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackcore/context_network.h"
#include "blackcore/context_ownaircraft.h"
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QScopedPointer>

namespace Ui { class CTextMessageComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Text message widget
        class CTextMessageComponent :
            public QTabWidget,
            public CEnableForRuntime
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
            ~CTextMessageComponent();

        signals:
            //! Message to be displayed in info window
            void displayInInfoWindow(const BlackMisc::CVariant &message, int displayDurationMs) const;

        public slots:
            //! \addtogroup commandline
            //! @{
            //! <pre>
            //! .m  .msg   message text  set transponder code    CTextMessageComponent
            //! </pre>
            //! @}
            //! \copydoc IContextOwnAircraft::parseCommandLine
            bool parseCommandLine(const QString &commandLine);

            //! Text messages received
            void onTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages) { this->textMessagesReceived(messages); }

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private:
            QScopedPointer<Ui::CTextMessageComponent> ui;
            QWidget   *getTabWidget(Tab tab); //!< enum to widget
            QAction   *m_clearTextEditAction = nullptr;
            QTextEdit *m_currentTextEdit     = nullptr;

            /*!
             * \brief Add new text message tab
             * \param tabName   name of the new tab, usually the channel name
             * \return
             */
            QWidget *addNewTextMessageTab(const QString &tabName);

            //! Find text message tab by its name
            QWidget *findTextMessageTabByName(const QString &name) const;

            /*!
             * \brief Private channel text message
             * \param textMessage
             * \param sending   sending or receiving
             */
            void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage, bool sending = false);

            /*!
             * Stub for sending a text message (eihter radio or private message).
             * Sets sender / receiver depending on frequency / channel situation.
             */
            BlackMisc::Network::CTextMessage getTextMessageStubForChannel();

            //! own aircraft
            const BlackMisc::Aviation::CAircraft getOwnAircraft() const { Q_ASSERT(this->getIContextOwnAircraft()); return this->getIContextOwnAircraft()->getOwnAircraft(); }

            //! For this text message's recepient, is the current tab selected?
            bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

            //! Network connected?
            bool isNetworkConnected() const { return this->getIContextNetwork() && this->getIContextNetwork()->isConnected() ; }

            //! Show current frequencies
            void showCurrentFrequenciesFromCockpit();

            /*!
             * \brief Append text messages (received, to be sent) to GUI
             * \param messages
             * \param sending
             */
            void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

        private slots:

            //! Cockpit values changed, used to updated some components
            void ps_onChangedAircraftCockpit();

            //! Close text message tab
            void ps_closeTextMessageTab();

            //! Context menu for text edit including clear
            void ps_showContextMenuForTextEdit(const QPoint &pt);

            //! Clear text edit
            void ps_clearTextEdit();
        };
    }
}
#endif // guard
