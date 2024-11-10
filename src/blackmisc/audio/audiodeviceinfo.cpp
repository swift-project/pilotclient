// SPDX-FileCopyrightText: Copyright (c) 2013 swift project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/verify.h"

#include <QStringBuilder>
#include <QHostInfo>
#include <QtGlobal>
#include <QAudioDevice>
#include <QMediaDevices>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Audio, CAudioDeviceInfo)

namespace BlackMisc::Audio
{
    CAudioDeviceInfo::CAudioDeviceInfo() : m_type(Unknown)
    {}

    CAudioDeviceInfo::CAudioDeviceInfo(DeviceType type, const QString &name) : m_type(static_cast<int>(type)),
                                                                               m_deviceName(name)
    {}

    bool CAudioDeviceInfo::isDefault() const
    {
        if (m_deviceName.isEmpty()) { return false; }
        if (m_deviceName == "default") { return true; }
        if (this->isInputDevice() && m_deviceName == QMediaDevices::defaultAudioInput().description()) { return true; }
        if (this->isOutputDevice() && m_deviceName == QMediaDevices::defaultAudioOutput().description()) { return true; }
        return false;
    }

    bool CAudioDeviceInfo::matchesNameTypeMachineName(const CAudioDeviceInfo &device) const
    {
        return device.getType() == this->getType() &&
               stringCompare(device.getName(), this->getName(), Qt::CaseInsensitive) &&
               stringCompare(device.getMachineName(), this->getMachineName(), Qt::CaseInsensitive);
    }

    bool CAudioDeviceInfo::matchesNameTypeMachineProcess(const CAudioDeviceInfo &device) const
    {
        return device.getType() == this->getType() &&
               device.getIdentifier().getProcessId() == this->getIdentifier().getProcessId() &&
               stringCompare(device.getName(), this->getName(), Qt::CaseInsensitive) &&
               stringCompare(device.getMachineName(), this->getMachineName(), Qt::CaseInsensitive);
    }

    CAudioDeviceInfo CAudioDeviceInfo::getDefaultOutputDevice()
    {
        return CAudioDeviceInfo(OutputDevice, QMediaDevices::defaultAudioOutput().description());
    }

    CAudioDeviceInfo CAudioDeviceInfo::getDefaultInputDevice()
    {
        return CAudioDeviceInfo(InputDevice, QMediaDevices::defaultAudioInput().description());
    }

    QVariant CAudioDeviceInfo::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDeviceType: return QVariant::fromValue(this->getType());
        case IndexDeviceTypeAsString: return QVariant::fromValue(this->getTypeAsString());
        case IndexName: return QVariant::fromValue(this->getName());
        case IndexIdentifier: return m_identifier.propertyByIndex(index.copyFrontRemoved());
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CAudioDeviceInfo::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAudioDeviceInfo>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDeviceType: m_type = static_cast<DeviceType>(variant.toInt()); return;
        case IndexName: m_deviceName = variant.toString(); return;
        case IndexIdentifier: m_identifier.setPropertyByIndex(index.copyFrontRemoved(), variant); return;
        default: break;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    int CAudioDeviceInfo::comparePropertyByIndex(CPropertyIndexRef index, const CAudioDeviceInfo &compareValue) const
    {
        if (index.isMyself()) { return m_deviceName.compare(compareValue.m_deviceName, Qt::CaseInsensitive); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDeviceTypeAsString:
        case IndexDeviceType: return Compare::compare(m_type, compareValue.m_type);
        case IndexName: return m_deviceName.compare(compareValue.m_deviceName, Qt::CaseInsensitive);
        case IndexIdentifier: return m_identifier.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIdentifier());
        default: break;
        }
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
        return 0;
    }

    const QString &CAudioDeviceInfo::deviceTypeToString(CAudioDeviceInfo::DeviceType t)
    {
        static const QString i("input");
        static const QString o("output");
        static const QString u("unknown");

        switch (t)
        {
        case InputDevice: return i;
        case OutputDevice: return o;
        default: break;
        }
        return u;
    }

    QString CAudioDeviceInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        if (!m_identifier.hasName()) { return m_deviceName; }
        return m_deviceName % u" [" % this->getMachineName() % u']';
    }

} // ns
