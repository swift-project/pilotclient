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
#include "blackmisc/identifier.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QAudioDeviceInfo>
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

            //! Find by name
            CAudioDeviceInfo findByNameOrDefault(const QString &name, const CAudioDeviceInfo defaultDevice, bool strict = false) const;

            //! Find by host name
            CAudioDeviceInfoList findByHostName(const QString &hostName) const;

            //! Find registered device
            CAudioDeviceInfo findRegisteredDeviceOrDefault(const CAudioDeviceInfo &device) const;

            //! Register device
            void registerDevice(const CAudioDeviceInfo &device);

            //! Register devices
            void registerDevices(const CAudioDeviceInfoList &devices);

            //! Un-register device
            void unRegisterDevice(const CAudioDeviceInfo &device);

            //! Un-register devices
            void unRegisterDevices(const CAudioDeviceInfoList &devices);

            //! Un-register devices
            void unRegisterDevices(const CIdentifier &identifier);

            //! Is that a registered device?
            bool isRegisteredDevice(const CAudioDeviceInfo &device) const;

            //! Count (as of type)
            int count(CAudioDeviceInfo::DeviceType type) const;

            //! All names
            QStringList getDeviceNames() const;

            //! Has same devices
            bool hasSameDevices(const CAudioDeviceInfoList &compareDevices) const;

            //! Lists of all available devices @{
            static CAudioDeviceInfoList allInputDevices();
            static CAudioDeviceInfoList allOutputDevices();
            static CAudioDeviceInfoList allDevices();
            static CAudioDeviceInfoList allInputDevicesPlusDefault();
            static CAudioDeviceInfoList allOutputDevicesPlusDefault();
            static CAudioDeviceInfoList allDevicesPlusDefault();
            static QList<QAudioDeviceInfo> allQtInputDevices();
            static QList<QAudioDeviceInfo> allQtOutputDevices();
            static QAudioDeviceInfo defaultQtInputDevice();
            static QAudioDeviceInfo defaultQtOutputDevice();
            static CAudioDeviceInfo defaultInputDevice();
            static CAudioDeviceInfo defaultOutputDevice();
            //! @}
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CAudioDeviceInfo>)

#endif //guard
