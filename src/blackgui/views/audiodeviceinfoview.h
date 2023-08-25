// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AUDIODEVICEINFOVIEW_H
#define BLACKGUI_VIEWS_AUDIODEVICEINFOVIEW_H

#include "blackgui/models/audiodeviceinfolistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    //! Audio device view
    class BLACKGUI_EXPORT CAudioDeviceInfoView : public CViewBase<Models::CAudioDeviceInfoListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioDeviceInfoView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
