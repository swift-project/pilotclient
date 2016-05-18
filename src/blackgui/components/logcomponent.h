/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LOGCOMPONENT_H
#define BLACKGUI_LOGCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/menus/menudelegate.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QString>

class QAction;
class QPoint;
class QWidget;

namespace Ui { class CLogComponent; }

namespace BlackGui
{
    namespace Menus { class CMenuActions; }

    namespace Components
    {
        //! Text edit for our log component
        class BLACKGUI_EXPORT CConsoleTextEdit : public QPlainTextEdit
        {
            Q_OBJECT

        public:
            //! Constructor
            CConsoleTextEdit(QWidget *parent = nullptr);

        protected slots:
            //! Custom menu
            void ps_customMenuRequested(const QPoint &pos);
        };

        //! GUI displaying log and status messages
        class BLACKGUI_EXPORT CLogComponent :
            public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLogComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CLogComponent();

            //! Display log
            void displayLog(bool attention = false);

            //! Display console
            void displayConsole(bool attention = false);

        signals:
            //! Make me visible
            void requestAttention();

        public slots:
            //! Append status message to console
            void appendStatusMessageToConsole(const BlackMisc::CStatusMessage &statusMessage);

            //! Append plain text to console
            void appendPlainTextToConsole(const QString &text);

            //! Append status message to list
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage);

            //! Append status messages to list
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages);

        private:
            QScopedPointer<Ui::CLogComponent> ui;

            //! Custom menu for the log component
            class CLogMenu : public BlackGui::Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLogMenu(CLogComponent *parent) : IMenuDelegate(parent) {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            private:
                QAction *m_action = nullptr;

            };
        };
    } // ns
} // ns
#endif // guard
