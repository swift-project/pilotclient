// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/audio/registermetadataaudio.h"

// Audio headers
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/audiosettings.h"

#include <QDBusMetaType>

namespace BlackMisc
{
    namespace Audio
    {
        //! Register metadata for audio / voice
        void registerMetadata()
        {
            CAudioDeviceInfo::registerMetadata();
            CAudioDeviceInfoList::registerMetadata();
            CSettings::registerMetadata();

            // ENUMs
            qDBusRegisterMetaType<CNotificationSounds::NotificationFlag>();
            qDBusRegisterMetaType<CAudioDeviceInfo::DeviceType>();
        }
    }

} // ns
