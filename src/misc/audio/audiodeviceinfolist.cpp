// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/audio/audiodeviceinfolist.h"

#include <algorithm>

#include <QMediaDevices>
#include <QString>

#include "misc/stringutils.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::audio, CAudioDeviceInfo, CAudioDeviceInfoList)

namespace swift::misc::audio
{
    CAudioDeviceInfoList::CAudioDeviceInfoList(const CSequence &other) : CSequence(other)
    {}

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
        return static_cast<int>(std::count_if(this->begin(), this->end(), [type](const CAudioDeviceInfo &device) {
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
        for (const QAudioDevice &inputDevice : allQtInputDevices())
        {
            const CAudioDeviceInfo d(CAudioDeviceInfo::InputDevice, inputDevice.description());
            if (!devices.contains(d)) { devices.push_back(d); }
        }
        return devices;
    }

    CAudioDeviceInfoList CAudioDeviceInfoList::allOutputDevices()
    {
        CAudioDeviceInfoList devices;
        for (const QAudioDevice &outputDevice : allQtOutputDevices())
        {
            const CAudioDeviceInfo d(CAudioDeviceInfo::OutputDevice, outputDevice.description());
            if (!devices.contains(d)) { devices.push_back(d); }
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

    QList<QAudioDevice> CAudioDeviceInfoList::allQtInputDevices()
    {
        const QList<QAudioDevice> devices = QMediaDevices::audioInputs();
        return devices;
    }

    QList<QAudioDevice> CAudioDeviceInfoList::allQtOutputDevices()
    {
        const QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
        return devices;
    }

    QAudioDevice CAudioDeviceInfoList::defaultQtInputDevice()
    {
        return QMediaDevices::defaultAudioInput();
    }

    QAudioDevice CAudioDeviceInfoList::defaultQtOutputDevice()
    {
        return QMediaDevices::defaultAudioOutput();
    }

    CAudioDeviceInfo CAudioDeviceInfoList::defaultInputDevice()
    {
        return CAudioDeviceInfo(CAudioDeviceInfo::InputDevice, defaultQtInputDevice().description());
    }

    CAudioDeviceInfo CAudioDeviceInfoList::defaultOutputDevice()
    {
        return CAudioDeviceInfo(CAudioDeviceInfo::OutputDevice, defaultQtOutputDevice().description());
    }

} // namespace swift::misc::audio
