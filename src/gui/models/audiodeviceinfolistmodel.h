// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AUDIODEVICEINFOLISTMODEL_H
#define SWIFT_GUI_MODELS_AUDIODEVICEINFOLISTMODEL_H

#include "gui/models/listmodelbase.h"
#include "gui/swiftguiexport.h"
#include "misc/audio/audiodeviceinfolist.h"

namespace swift::gui::models
{
    //! Audio device list model
    class SWIFT_GUI_EXPORT CAudioDeviceInfoListModel :
        public CListModelBase<swift::misc::audio::CAudioDeviceInfoList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioDeviceInfoListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAudioDeviceInfoListModel() override {}
    };
} // namespace swift::gui::models
#endif // SWIFT_GUI_MODELS_AUDIODEVICEINFOLISTMODEL_H
