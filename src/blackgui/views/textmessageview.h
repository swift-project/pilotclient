// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_TEXTMESSAGEVIEW_H
#define BLACKGUI_TEXTMESSAGEVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/textmessagelistmodel.h"
#include "misc/network/textmessagelist.h"
#include "blackgui/blackguiexport.h"

namespace swift::misc::network
{
    class CTextMessage;
}
namespace BlackGui::Views
{
    //! Airports view
    class BLACKGUI_EXPORT CTextMessageView : public CViewBase<Models::CTextMessageListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTextMessageView(QWidget *parent = nullptr);

        //! Set display mode
        void setTextMessageMode(BlackGui::Models::CTextMessageListModel::TextMessageMode mode);

        //! Sorted by a timestamp property
        bool isSortedByTimestampProperty() const;

        //! Sorted by a timestamp property, latest messages at the end
        bool isSortedByTimestampPropertyLatestLast() const;
    };
} // namespace

#endif // guard
