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

#include <QString>
#include <QAudioDeviceInfo>
#include <QHostInfo>
#include <algorithm>
#include <tuple>

namespace BlackMisc
{
    namespace Audio
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

        CAudioDeviceInfo CAudioDeviceInfoList::findByNameOrDefault(const QString &name, const CAudioDeviceInfo defaultDevice, bool strict) const
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

        CAudioDeviceInfoList CAudioDeviceInfoList::allInputDevices()
        {
            const QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
            CAudioDeviceInfoList devices;

            for (const QAudioDeviceInfo &inputDevice : inputDevices)
            {
                const CAudioDeviceInfo d(CAudioDeviceInfo::InputDevice, inputDevice.deviceName());
                if (! devices.contains(d)) { devices.push_back(d); }
            }
            return devices;
        }

        CAudioDeviceInfoList CAudioDeviceInfoList::allOutputDevices()
        {
            const QList<QAudioDeviceInfo> outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
            CAudioDeviceInfoList devices;

            for (const QAudioDeviceInfo &outputDevice : outputDevices)
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

    } // namespace
} // namespace
