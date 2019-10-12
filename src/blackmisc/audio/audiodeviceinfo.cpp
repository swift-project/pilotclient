/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/stringutils.h"

#include <QStringBuilder>
#include <QHostInfo>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Audio
    {
        CAudioDeviceInfo::CAudioDeviceInfo() :
            m_type(Unknown),
            m_hostName(QHostInfo::localHostName())
        { }

        CAudioDeviceInfo::CAudioDeviceInfo(DeviceType type, const QString &name) :
            m_type(type),
            m_deviceName(name), m_hostName(QHostInfo::localHostName())
        { }

        bool CAudioDeviceInfo::matchesNameTypeHostName(const CAudioDeviceInfo &device) const
        {
            return device.getType() == this->getType() &&
                   stringCompare(device.getName(), this->getName(), Qt::CaseInsensitive) &&
                   stringCompare(device.getHostName(), this->getHostName(), Qt::CaseInsensitive);
        }

        CAudioDeviceInfo::DeviceType CAudioDeviceInfo::fromQtMode(QAudio::Mode m)
        {
            switch (m)
            {
            case QAudio::AudioInput:  return InputDevice;
            case QAudio::AudioOutput: return OutputDevice;
            default: break;
            }
            return Unknown;
        }

        QString CAudioDeviceInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n)
            if (m_hostName.isEmpty()) { return m_deviceName; }
            return m_deviceName % u" [" % this->getHostName() % u']';
        }
    } // ns
} // ns
