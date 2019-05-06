/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AUDIODEVICE_H
#define BLACKCORE_AUDIODEVICE_H

#include "blackcoreexport.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/audiodeviceinfolist.h"

#include <QObject>

namespace BlackCore
{
    //! Audio Input Device
    //! \todo Settings classes to store hardware settings (hardware device)
    class BLACKCORE_EXPORT IAudioInputDevice : public QObject
    {
        Q_OBJECT

    public:

        //! Constructor
        IAudioInputDevice(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IAudioInputDevice() {}

        //! Get available input devices
        virtual const BlackMisc::Audio::CAudioDeviceInfoList &getInputDevices() const = 0;

        //! Current input device
        virtual const BlackMisc::Audio::CAudioDeviceInfo &getCurrentInputDevice() const = 0;

        //! Set new input device
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) = 0;
    };

    class BLACKCORE_EXPORT CAudioInputDeviceDummy : public IAudioInputDevice
    {
        Q_OBJECT
    public:
        //! Constructor
        CAudioInputDeviceDummy(QObject *parent = nullptr) : IAudioInputDevice(parent) {}

        //! Destructor
        virtual ~CAudioInputDeviceDummy() override = default;

        //! \copydoc IAudioInputDevice::getInputDevices
        virtual const BlackMisc::Audio::CAudioDeviceInfoList &getInputDevices() const override { return m_devices; }

        //! \copydoc IAudioInputDevice::getCurrentInputDevice
        virtual const BlackMisc::Audio::CAudioDeviceInfo &getCurrentInputDevice() const override { return m_currentDevice; }

        //! \copydoc IAudioInputDevice::setInputDevice
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) override { m_currentDevice = device; }

    private:
        BlackMisc::Audio::CAudioDeviceInfoList m_devices; /*!< in and output devices */
        BlackMisc::Audio::CAudioDeviceInfo m_currentDevice;
    };

    //! Audio Output Device
    class IAudioOutputDevice : public QObject
    {
        Q_OBJECT

    public:

        //! Constructor
        IAudioOutputDevice(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IAudioOutputDevice() {}

        //! Get available output devices
        virtual const BlackMisc::Audio::CAudioDeviceInfoList &getOutputDevices() const = 0;

        //! Current output device
        virtual const BlackMisc::Audio::CAudioDeviceInfo &getCurrentOutputDevice() const = 0;

        //! Set new output device
        virtual void setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) = 0;

        //! Set output volume between 0 ... 300%
        virtual void setOutputVolume(int volume) = 0;

        //! Get output volume between 0 ... 300%
        virtual int getOutputVolume() const = 0;
    };
}

#endif // guard
