// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/audio/registermetadataaudio.h"

// Audio headers
#include "misc/audio/audiodeviceinfo.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/audio/audiosettings.h"

#include <QDBusMetaType>

namespace swift::misc
{
    namespace audio
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
