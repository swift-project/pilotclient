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
    namespace Components
    {
        //! Text edit for our log component
        class BLACKGUI_EXPORT CConsoleTextEdit : public QPlainTextEdit
        {
            Q_OBJECT

        public:
            //! Constructor
            CConsoleTextEdit(QWidget *parent = nullptr);

        protected:
            //! Custom menu
            void customMenuRequested(const QPoint &pos);
        };

        //! GUI displaying log and status messages
        class BLACKGUI_EXPORT CLogComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLogComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CLogComponent();

            //! Display log
            void displayLog(bool attention = false);

            //! Display console
            void displayConsole(bool attention = false);

            //! \copydoc BlackGui::Components::CStatusMessagesDetail::filterUseRadioButtonDescriptiveIcons
            void filterUseRadioButtonDescriptiveIcons(bool oneLetterText);

            //! \copydoc BlackGui::Components::CStatusMessagesDetail::showFilterDialog
            void showFilterDialog();

            //! \copydoc BlackGui::Components::CStatusMessagesDetail::showFilterBar
            void showFilterBar();

            //! \copydoc BlackGui::Components::CStatusMessagesDetail::showDetails
            void showDetails(bool details);

            //! \copydoc BlackGui::Components::CStatusMessagesDetail::setMaxLogMessages
            void setMaxLogMessages(int max);

            //! Clear
            void clear();

            //! Append status message to console
            void appendStatusMessageToConsole(const BlackMisc::CStatusMessage &statusMessage);

            //! Append plain text to console
            void appendPlainTextToConsole(const QString &text);

            //! Append status message to list
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage);

            //! Append status messages to list
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages);

        signals:
            //! Make me visible
            void requestAttention();

        private:
            QScopedPointer<Ui::CLogComponent> ui;

            //! Status messages changed
            void onStatusMessageDataChanged(int count, bool withFilter);
        };
    } // ns
} // ns
#endif // guard
