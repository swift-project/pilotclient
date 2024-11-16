// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_CLIENTVIEW_H
#define SWIFT_GUI_VIEWS_CLIENTVIEW_H

#include "gui/models/clientlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"
#include "misc/network/clientlist.h"

namespace swift::misc::network
{
    class CClient;
}
namespace swift::gui::views
{
    //! Client view
    class SWIFT_GUI_EXPORT CClientView : public CViewBase<models::CClientListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CClientView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // guard
