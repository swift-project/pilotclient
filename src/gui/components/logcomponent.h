// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_LOGCOMPONENT_H
#define SWIFT_GUI_LOGCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/loghistory.h"
#include "misc/statusmessagelist.h"

class QAction;
class QPoint;
class QWidget;

namespace Ui
{
    class CLogComponent;
}
namespace swift::gui::components
{
    //! Text edit for our log component
    class SWIFT_GUI_EXPORT CConsoleTextEdit : public QPlainTextEdit
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
    class SWIFT_GUI_EXPORT CLogComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLogComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLogComponent();

        //! Display log
        void displayLog(bool attention = false);

        //! \copydoc swift::gui::components::CStatusMessagesDetail::filterUseRadioButtonDescriptiveIcons
        void filterUseRadioButtonDescriptiveIcons(bool oneLetterText);

        //! \copydoc swift::gui::components::CStatusMessagesDetail::showFilterDialog
        void showFilterDialog();

        //! \copydoc swift::gui::components::CStatusMessagesDetail::showFilterBar
        void showFilterBar();

        //! \copydoc swift::gui::components::CStatusMessagesDetail::hideFilterBar
        void hideFilterBar();

        //! \copydoc swift::gui::components::CStatusMessagesDetail::showDetails
        void showDetails(bool details);

        //! \copydoc swift::gui::components::CStatusMessagesDetail::setMaxLogMessages
        void setMaxLogMessages(int max);

        //! \copydoc swift::gui::components::CStatusMessagesDetail::setNoSorting
        void setNoSorting();

        //! \copydoc swift::gui::components::CStatusMessagesDetail::setSorting
        void setSorting(const swift::misc::CPropertyIndex &propertyIndex, Qt::SortOrder order);

        //! Clear
        void clear();

        //! Clear
        void clearMessages();

    signals:
        //! Make me visible
        void requestAttention();

    private:
        QScopedPointer<Ui::CLogComponent> ui;
        swift::misc::CLogHistoryReplica m_history;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_LOGCOMPONENT_H
