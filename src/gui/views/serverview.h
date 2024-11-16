// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_SERVERVIEW_H
#define SWIFT_GUI_VIEWS_SERVERVIEW_H

#include "gui/models/serverlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

class QWidget;

namespace swift::gui::views
{
    //! Network servers
    class SWIFT_GUI_EXPORT CServerView : public CViewBase<models::CServerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // guard
