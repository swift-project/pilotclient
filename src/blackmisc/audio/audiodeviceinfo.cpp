/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "audiodeviceinfo.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>
#include <QHostInfo>

namespace BlackMisc
{
    namespace Audio
    {

        CAudioDeviceInfo::CAudioDeviceInfo() :
            m_type(Unknown), m_deviceIndex(invalidDeviceIndex()),
            m_deviceName(""), m_hostName(QHostInfo::localHostName())
        { }

        CAudioDeviceInfo::CAudioDeviceInfo(DeviceType type, const int index, const QString &name) :
            m_type(type), m_deviceIndex(index),
            m_deviceName(name), m_hostName(QHostInfo::localHostName())
        { }

        QString CAudioDeviceInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            if (this->m_hostName.isEmpty()) return m_deviceName;
            QString s(this->m_deviceName);
            s.append(" [");
            s.append(this->getHostName());
            s.append("]");
            return s;
        }

    } // Voice
} // BlackMisc
