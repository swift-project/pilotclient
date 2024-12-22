// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_TEXTMESSAGEVIEW_H
#define SWIFT_GUI_TEXTMESSAGEVIEW_H

#include "gui/models/textmessagelistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

namespace swift::misc::network
{
    class CTextMessage;
}
namespace swift::gui::views
{
    //! Airports view
    class SWIFT_GUI_EXPORT CTextMessageView : public CViewBase<models::CTextMessageListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTextMessageView(QWidget *parent = nullptr);

        //! Set display mode
        void setTextMessageMode(swift::gui::models::CTextMessageListModel::TextMessageMode mode);

        //! Sorted by a timestamp property
        bool isSortedByTimestampProperty() const;

        //! Sorted by a timestamp property, latest messages at the end
        bool isSortedByTimestampPropertyLatestLast() const;
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_TEXTMESSAGEVIEW_H
