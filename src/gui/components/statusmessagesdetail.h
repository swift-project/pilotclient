// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_STATUSMESSAGESDETAIL_H
#define SWIFT_GUI_COMPONENTS_STATUSMESSAGESDETAIL_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/menus/menudelegate.h"
#include "gui/swiftguiexport.h"
#include "misc/digestsignal.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

namespace Ui
{
    class CStatusMessagesDetail;
}
namespace swift::gui::components
{
    //! Status messages plus detail
    class SWIFT_GUI_EXPORT CStatusMessagesDetail : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CStatusMessagesDetail(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessagesDetail() override;

        //! Add message
        void appendStatusMessageToList(const swift::misc::CStatusMessage &message);

        //! Add messages
        void appendStatusMessagesToList(const swift::misc::CStatusMessageList &messages);

        //! Show log details
        void showDetails(bool details);

        //! Show filter dialog and disable bar
        void showFilterDialog();

        //! Show a filter bar
        void showFilterBar();

        //! Hide a filter bar
        void hideFilterBar();

        //! Clear
        void clear();

        //! Number of status messages in view
        int rowCount() const;

        //! Set max.log messages
        void setMaxLogMessages(int desiredNumber) { m_maxLogMessages = desiredNumber; }

        //! \copydoc swift::gui::filters::CStatusMessageFilterBar::useRadioButtonDescriptiveIcons
        void filterUseRadioButtonDescriptiveIcons(bool oneCharacterText);

        //! Sorting for view
        void setSorting(const swift::misc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

        //! Disable sorting
        void setNoSorting();

    signals:
        //! \copydoc swift::gui::views::CStatusMessageView::modelDataChangedDigest
        void modelDataChangedDigest(int count, bool withFilter);

        //! The user changed their message filter
        void filterChanged(const swift::misc::CVariant &filter);

    private:
        QScopedPointer<Ui::CStatusMessagesDetail> ui;
        int m_maxLogMessages = -1;
        swift::misc::CStatusMessageList
            m_pending; //!< pending messages which will be added with next CStatusMessagesDetail::deferredUpdate
        swift::misc::CDigestSignal m_dsDeferredUpdate { this, &CStatusMessagesDetail::deferredUpdate, 2000, 25 };

        //! Do not update each message, but deferred
        void deferredUpdate();

        //! Custom menu for the log component
        //! \fixme Move to namespace scope and add Q_OBJECT
        class CMessageMenu : public menus::IMenuDelegate
        {
        public:
            //! Constructor
            CMessageMenu(CStatusMessagesDetail *parent) : IMenuDelegate(parent) {}

            //! \copydoc IMenuDelegate::customMenu
            virtual void customMenu(menus::CMenuActions &menuActions) override;

        private:
            QAction *m_action = nullptr;
        };
    };
} // namespace swift::gui::components

#endif // guard
