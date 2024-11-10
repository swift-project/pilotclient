// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AUDIODEVICEINFOLISTMODEL_H
#define BLACKGUI_MODELS_AUDIODEVICEINFOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/audio/audiodeviceinfolist.h"

namespace BlackGui::Models
{
    //! Audio device list model
    class BLACKGUI_EXPORT CAudioDeviceInfoListModel :
        public CListModelBase<swift::misc::audio::CAudioDeviceInfoList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioDeviceInfoListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAudioDeviceInfoListModel() override {}
    };
}
#endif // guard
