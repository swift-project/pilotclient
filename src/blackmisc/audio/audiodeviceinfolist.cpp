/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"
#include "blackmisc/stringutils.h"

#include <QString>
#include <QAudioDeviceInfo>
#include <QHostInfo>
#include <algorithm>
#include <tuple>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Audio, CAudioDeviceInfo, CAudioDeviceInfoList)

namespace BlackMisc::Audio
{
    CAudioDeviceInfoList::CAudioDeviceInfoList() { }

    CAudioDeviceInfoList::CAudioDeviceInfoList(const CSequence &other) :
        CSequence(other)
    { }

    CAudioDeviceInfoList CAudioDeviceInfoList::getOutputDevices() const
    {
        return this->findBy(&CAudioDeviceInfo::getType, CAudioDeviceInfo::OutputDevice);
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::getInputDevices() const
    {
        return this->findBy(&CAudioDeviceInfo::getType, CAudioDeviceInfo::InputDevice);
    }

    CAudioDeviceInfo CAudioDeviceInfoList::findByName(const QString &name, bool strict) const
    {
        return this->findByNameOrDefault(name, CAudioDeviceInfo(), strict);
    }

    CAudioDeviceInfo CAudioDeviceInfoList::findByNameOrDefault(const QString &name, const CAudioDeviceInfo &defaultDevice, bool strict) const
    {
        if (name.isEmpty()) { return defaultDevice; }
        for (const CAudioDeviceInfo &d : *this)
        {
            if (strict)
            {
                if (d.getName() == name) { return d; }
            }
            else
            {
                if (d.getName().startsWith(name, Qt::CaseInsensitive)) { return d; }
                if (name.startsWith(d.getName(), Qt::CaseInsensitive)) { return d; }
            }
        }
        return defaultDevice;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::findByHostName(const QString &hostName) const
    {
        if (hostName.isEmpty()) { return {}; }

        CAudioDeviceInfoList devices;
        for (const CAudioDeviceInfo &d : *this)
        {
            if (stringCompare(hostName, d.getMachineName(), Qt::CaseInsensitive))
            {
                devices.push_back(d);
            }
        }
        return devices;
    }

    CAudioDeviceInfo CAudioDeviceInfoList::findRegisteredDeviceOrDefault(const CAudioDeviceInfo &device) const
    {
        for (const CAudioDeviceInfo &d : *this)
        {
            if (device.matchesNameTypeMachineProcess(d)) { return d; }
        }
        return {};
    }

    void CAudioDeviceInfoList::registerDevice(const CAudioDeviceInfo &device)
    {
        if (!device.isValid()) { return; }
        if (this->isRegisteredDevice(device)) { return; }
        this->push_back(device);
    }

    void CAudioDeviceInfoList::registerDevices(const CAudioDeviceInfoList &devices)
    {
        for (const CAudioDeviceInfo &device : devices)
        {
            this->registerDevice(device);
        }
    }

    void CAudioDeviceInfoList::unRegisterDevice(const CAudioDeviceInfo &device)
    {
        if (!device.isValid()) { return; }
        const CAudioDeviceInfo registeredDevice = this->findRegisteredDeviceOrDefault(device);
        if (registeredDevice.isValid())
        {
            this->remove(registeredDevice);
        }
    }

    void CAudioDeviceInfoList::unRegisterDevices(const CAudioDeviceInfoList &devices)
    {
        for (const CAudioDeviceInfo &device : devices)
        {
            this->unRegisterDevice(device);
        }
    }

    void CAudioDeviceInfoList::unRegisterDevices(const CIdentifier &identifier)
    {
        this->removeIf(&CAudioDeviceInfo::getIdentifier, identifier);
    }

    bool CAudioDeviceInfoList::isRegisteredDevice(const CAudioDeviceInfo &device) const
    {
        return this->findRegisteredDeviceOrDefault(device).isValid();
    }

    int CAudioDeviceInfoList::count(CAudioDeviceInfo::DeviceType type) const
    {
        return static_cast<int>(std::count_if(this->begin(), this->end(), [type](const CAudioDeviceInfo & device)
        {
            return device.getType() == type;
        }));
    }

    QStringList CAudioDeviceInfoList::getDeviceNames() const
    {
        QStringList names;
        for (const CAudioDeviceInfo &d : *this)
        {
            names << d.getName();
        }
        return names;
    }

    bool CAudioDeviceInfoList::hasSameDevices(const CAudioDeviceInfoList &compareDevices) const
    {
        if (compareDevices.size() != this->size()) { return false; }
        for (const CAudioDeviceInfo &d : *this)
        {
            if (!compareDevices.findRegisteredDeviceOrDefault(d).isValid()) { return false; }
        }
        return true;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allInputDevices()
    {
        CAudioDeviceInfoList devices;
        for (const QAudioDeviceInfo &inputDevice : allQtInputDevices())
        {
            const CAudioDeviceInfo d(CAudioDeviceInfo::InputDevice, inputDevice.deviceName());
            if (! devices.contains(d)) { devices.push_back(d); }
        }
        return devices;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allOutputDevices()
    {
        CAudioDeviceInfoList devices;
        for (const QAudioDeviceInfo &outputDevice : allQtOutputDevices())
        {
            const CAudioDeviceInfo d(CAudioDeviceInfo::OutputDevice, outputDevice.deviceName());
            if (! devices.contains(d)) { devices.push_back(d); }
        }
        return devices;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allDevices()
    {
        CAudioDeviceInfoList i = allInputDevices();
        i.push_back(allOutputDevices());
        return i;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allInputDevicesPlusDefault()
    {
        CAudioDeviceInfoList i = allInputDevices();
        i.push_back(CAudioDeviceInfoList::defaultInputDevice());
        return i;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allOutputDevicesPlusDefault()
    {
        CAudioDeviceInfoList o = allOutputDevices();
        o.push_back(CAudioDeviceInfoList::defaultOutputDevice());
        return o;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allDevicesPlusDefault()
    {
        CAudioDeviceInfoList i = allInputDevicesPlusDefault();
        i.push_back(allOutputDevicesPlusDefault());
        return i;
    }

    QList<QAudioDeviceInfo> CAudioDeviceInfoList::allQtInputDevices()
    {
        const QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        return devices;
    }

    QList<QAudioDeviceInfo> CAudioDeviceInfoList::allQtOutputDevices()
    {
        const QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        return devices;
    }

    QAudioDeviceInfo CAudioDeviceInfoList::defaultQtInputDevice()
    {
        return QAudioDeviceInfo::defaultInputDevice();
    }

    QAudioDeviceInfo CAudioDeviceInfoList::defaultQtOutputDevice()
    {
        return QAudioDeviceInfo::defaultOutputDevice();
    }

    CAudioDeviceInfo CAudioDeviceInfoList::defaultInputDevice()
    {
        return CAudioDeviceInfo(CAudioDeviceInfo::InputDevice, defaultQtInputDevice().deviceName());
    }

    CAudioDeviceInfo CAudioDeviceInfoList::defaultOutputDevice()
    {
        return CAudioDeviceInfo(CAudioDeviceInfo::OutputDevice, defaultQtOutputDevice().deviceName());
    }

} // namespace
