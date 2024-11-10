// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_STATUSMESSAGEVIEW_H
#define SWIFT_GUI_VIEWS_STATUSMESSAGEVIEW_H

#include "gui/views/viewbase.h"
#include "gui/models/statusmessagelistmodel.h"
#include "misc/statusmessagelist.h"
#include "misc/statusmessage.h"
#include "gui/swiftguiexport.h"

#include <QObject>

namespace swift::gui
{
    namespace filters
    {
        class CStatusMessageFilterDialog;
    }
    namespace views
    {
        //! Status message view
        class SWIFT_GUI_EXPORT CStatusMessageView : public CViewBase<models::CStatusMessageListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CStatusMessageView(QWidget *parent = nullptr);

            //! Set mode
            void setMode(models::CStatusMessageListModel::Mode mode);

            //! Set mode and adjust order/no order
            void setMode(models::CStatusMessageListModel::Mode mode, const swift::misc::CStatusMessageList &messages);

            //! \copydoc swift::gui::models::CStatusMessageListModel::adjustOrderColumn
            void adjustOrderColumn(const swift::misc::CStatusMessageList &messages);

            //! \copydoc swift::gui::models::CStatusMessageListModel::isSortedByTimestampOrOrder
            bool isSortedByTimestampOrOrder() const;

            //! \copydoc swift::misc::CStatusMessageList::keepLatest
            void keepLatest(int desiredSize);

            //! Add my own filter dialog
            void addFilterDialog();

            //! Filter dialog if any
            filters::CStatusMessageFilterDialog *getFilterDialog() const;
        };
    } // ns
} // ns
#endif // guard
