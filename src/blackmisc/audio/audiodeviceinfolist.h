/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_AUDIODEVICELIST_H
#define BLACKMISC_AUDIO_AUDIODEVICELIST_H

#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QStringList>

namespace BlackMisc
{
    namespace Audio
    {
        //! Value object encapsulating a list of audio devices.
        class BLACKMISC_EXPORT CAudioDeviceInfoList :
            public CSequence<CAudioDeviceInfo>,
            public Mixin::MetaType<CAudioDeviceInfoList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAudioDeviceInfoList)
            using CSequence::CSequence;

            //! Default constructor.
            CAudioDeviceInfoList();

            //! Construct from a base class object.
            CAudioDeviceInfoList(const CSequence &other);

            //! Get output devices in that list
            CAudioDeviceInfoList getOutputDevices() const;

            //! Get output devices in that list
            CAudioDeviceInfoList getInputDevices() const;

            //! Find by name
            CAudioDeviceInfo findByName(const QString &name, bool strict = false) const;

            //! Find device by its index
            CAudioDeviceInfo findByDeviceIndex(int deviceIndex);

            //! Count (as of type)
            int count(CAudioDeviceInfo::DeviceType type) const;

            //! All names
            QStringList getDeviceNames() const;

            //! List based on the Qt devices @{
            static CAudioDeviceInfoList allQtInputDevices();
            static CAudioDeviceInfoList allQtOutputDevices();
            static CAudioDeviceInfoList allQtDevices();
            static CAudioDeviceInfo fromQtInputDevice(const QAudioDeviceInfo &device);
            static CAudioDeviceInfo fromQtOutputDevice(const QAudioDeviceInfo &device);
            static CAudioDeviceInfo qtDefaultInputDevice();
            static CAudioDeviceInfo qtDefaultOutputDevice();
            //! @}
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CAudioDeviceInfo>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Audio::CAudioDeviceInfo>)

#endif //guard
