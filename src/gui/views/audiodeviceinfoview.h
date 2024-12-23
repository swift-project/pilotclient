// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AUDIODEVICEINFOVIEW_H
#define SWIFT_GUI_VIEWS_AUDIODEVICEINFOVIEW_H

#include "gui/models/audiodeviceinfolistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

namespace swift::gui::views
{
    //! Audio device view
    class SWIFT_GUI_EXPORT CAudioDeviceInfoView : public CViewBase<models::CAudioDeviceInfoListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioDeviceInfoView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_VIEWS_AUDIODEVICEINFOVIEW_H
