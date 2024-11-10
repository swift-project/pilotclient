// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_STATUSMESSAGEVIEW_H
#define BLACKGUI_VIEWS_STATUSMESSAGEVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "misc/statusmessagelist.h"
#include "misc/statusmessage.h"
#include "blackgui/blackguiexport.h"

#include <QObject>

namespace BlackGui
{
    namespace Filters
    {
        class CStatusMessageFilterDialog;
    }
    namespace Views
    {
        //! Status message view
        class BLACKGUI_EXPORT CStatusMessageView : public CViewBase<Models::CStatusMessageListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CStatusMessageView(QWidget *parent = nullptr);

            //! Set mode
            void setMode(Models::CStatusMessageListModel::Mode mode);

            //! Set mode and adjust order/no order
            void setMode(Models::CStatusMessageListModel::Mode mode, const swift::misc::CStatusMessageList &messages);

            //! \copydoc BlackGui::Models::CStatusMessageListModel::adjustOrderColumn
            void adjustOrderColumn(const swift::misc::CStatusMessageList &messages);

            //! \copydoc BlackGui::Models::CStatusMessageListModel::isSortedByTimestampOrOrder
            bool isSortedByTimestampOrOrder() const;

            //! \copydoc swift::misc::CStatusMessageList::keepLatest
            void keepLatest(int desiredSize);

            //! Add my own filter dialog
            void addFilterDialog();

            //! Filter dialog if any
            Filters::CStatusMessageFilterDialog *getFilterDialog() const;
        };
    } // ns
} // ns
#endif // guard
