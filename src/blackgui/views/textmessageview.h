/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEXTMESSAGEVIEW_H
#define BLACKGUI_TEXTMESSAGEVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/textmessagelistmodel.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackgui/blackguiexport.h"

namespace BlackMisc::Network { class CTextMessage; }
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
