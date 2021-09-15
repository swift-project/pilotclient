/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AUDIODEVICEINFOLISTMODEL_H
#define BLACKGUI_MODELS_AUDIODEVICEINFOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/audio/audiodeviceinfolist.h"

namespace BlackGui::Models
{
    //! Audio device list model
    class BLACKGUI_EXPORT CAudioDeviceInfoListModel :
        public CListModelBase<BlackMisc::Audio::CAudioDeviceInfoList, true>
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
