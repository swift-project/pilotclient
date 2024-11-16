// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_IDENTIFIERVIEW_H
#define SWIFT_GUI_IDENTIFIERVIEW_H

#include "gui/models/identifierlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"
#include "misc/identifierlist.h"

namespace swift::misc
{
    class CIdentifier;
}
namespace swift::gui::views
{
    //! Originator servers
    class SWIFT_GUI_EXPORT CIdentifierView : public CViewBase<models::CIdentifierListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIdentifierView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // guard
