/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
