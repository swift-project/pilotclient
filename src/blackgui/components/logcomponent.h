// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_LOGCOMPONENT_H
#define BLACKGUI_LOGCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/loghistory.h"
#include "blackmisc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QString>

class QAction;
class QPoint;
class QWidget;

namespace Ui
{
    class CLogComponent;
}
namespace BlackGui::Components
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

        //! \copydoc BlackGui::Components::CStatusMessagesDetail::hideFilterBar
        void hideFilterBar();

        //! \copydoc BlackGui::Components::CStatusMessagesDetail::showDetails
        void showDetails(bool details);

        //! \copydoc BlackGui::Components::CStatusMessagesDetail::setMaxLogMessages
        void setMaxLogMessages(int max);

        //! \copydoc BlackGui::Components::CStatusMessagesDetail::setNoSorting
        void setNoSorting();

        //! \copydoc BlackGui::Components::CStatusMessagesDetail::setSorting
        void setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order);

        //! Clear
        void clear();

        //! Clear
        void clearConsole();

        //! Clear
        void clearMessages();

        //! Append plain text to console
        void appendPlainTextToConsole(const QString &text);

    signals:
        //! Make me visible
        void requestAttention();

    private:
        QScopedPointer<Ui::CLogComponent> ui;
        BlackMisc::CLogHistoryReplica m_history;

        //! Status messages changed
        void onStatusMessageDataChanged(int count, bool withFilter);
    };
} // ns
#endif // guard
