// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_WINDLAYERVIEW_H
#define SWIFT_GUI_WINDLAYERVIEW_H

#include "gui/models/windlayerlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

class QWidget;

namespace swift::misc::weather
{
    class CWindLayer;
}
namespace swift::gui::views
{
    //! Wind layer view
    class SWIFT_GUI_EXPORT CWindLayerView : public CViewBase<models::CWindLayerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CWindLayerView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // guard
