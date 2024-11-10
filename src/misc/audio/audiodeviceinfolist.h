// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AUDIO_AUDIODEVICELIST_H
#define SWIFT_MISC_AUDIO_AUDIODEVICELIST_H

#include "misc/audio/audiodeviceinfo.h"
#include "misc/identifier.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

#include <QMetaType>
#include <QAudioDevice>
#include <QStringList>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::audio, CAudioDeviceInfo, CAudioDeviceInfoList)

namespace swift::misc::audio
{
    //! Value object encapsulating a list of audio devices.
    class SWIFT_MISC_EXPORT CAudioDeviceInfoList :
        public CSequence<CAudioDeviceInfo>,
        public mixin::MetaType<CAudioDeviceInfoList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAudioDeviceInfoList)
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
        CAudioDeviceInfo findByNameOrDefault(const QString &name, const CAudioDeviceInfo &defaultDevice, bool strict = false) const;

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

        //! @{
        //! Lists of all available devices
        static CAudioDeviceInfoList allInputDevices();
        static CAudioDeviceInfoList allOutputDevices();
        static CAudioDeviceInfoList allDevices();
        static CAudioDeviceInfoList allInputDevicesPlusDefault();
        static CAudioDeviceInfoList allOutputDevicesPlusDefault();
        static CAudioDeviceInfoList allDevicesPlusDefault();
        static QList<QAudioDevice> allQtInputDevices();
        static QList<QAudioDevice> allQtOutputDevices();
        static QAudioDevice defaultQtInputDevice();
        static QAudioDevice defaultQtOutputDevice();
        static CAudioDeviceInfo defaultInputDevice();
        static CAudioDeviceInfo defaultOutputDevice();
        //! @}
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::audio::CAudioDeviceInfoList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::audio::CAudioDeviceInfo>)

#endif // guard
