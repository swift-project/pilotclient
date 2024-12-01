// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_CLOUDLAYERVIEW_H
#define SWIFT_GUI_CLOUDLAYERVIEW_H

#include "gui/models/cloudlayerlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

class QWidget;

namespace swift::misc::weather
{
    class CCloudLayer;
}

namespace swift::gui::views
{
    //! Airports view
    class SWIFT_GUI_EXPORT CCloudLayerView : public CViewBase<models::CCloudLayerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCloudLayerView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // SWIFT_GUI_CLOUDLAYERVIEW_H
