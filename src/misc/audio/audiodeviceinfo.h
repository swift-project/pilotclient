// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AUDIO_AUDIODEVICE_H
#define SWIFT_MISC_AUDIO_AUDIODEVICE_H

#include <QMetaType>
#include <QString>

#include "misc/identifier.h"
#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::audio, CAudioDeviceInfo)

namespace swift::misc::audio
{
    /*!
     * Value object encapsulating information of a audio device.
     * If you want to safe this object, use the name instead of the index, since the index can change after
     * a restart.
     */
    class SWIFT_MISC_EXPORT CAudioDeviceInfo : public CValueObject<CAudioDeviceInfo>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = CPropertyIndexRef::GlobalIndexCAudioDeviceInfo,
            IndexDeviceType,
            IndexDeviceTypeAsString,
            IndexIdentifier
        };

        //! Type
        enum DeviceType
        {
            InputDevice,
            OutputDevice,
            Unknown
        };

        //! Default constructor.
        CAudioDeviceInfo();

        //! Constructor.
        CAudioDeviceInfo(DeviceType type, const QString &name);

        //! Get the device name
        const QString &getName() const { return m_deviceName; }

        //! Machine name
        const QString &getMachineName() const { return m_identifier.getMachineName(); }

        //! Identifier
        const CIdentifier &getIdentifier() const { return m_identifier; }

        //! Type
        DeviceType getType() const { return static_cast<DeviceType>(m_type); }

        //! Type as string
        const QString &getTypeAsString() const { return deviceTypeToString(this->getType()); }

        //! Input device
        bool isInputDevice() const { return this->getType() == InputDevice; }

        //! Output device
        bool isOutputDevice() const { return this->getType() == OutputDevice; }

        //! Valid audio device object?
        bool isValid() const { return !m_deviceName.isEmpty() && (m_type != Unknown); }

        //! Is this a default device?
        bool isDefault() const;

        //! Matching name, type and machine
        bool matchesNameTypeMachineName(const CAudioDeviceInfo &device) const;

        //! Matching name, type and machine and process
        bool matchesNameTypeMachineProcess(const CAudioDeviceInfo &device) const;

        //! Default output device
        static CAudioDeviceInfo getDefaultOutputDevice();

        //! Default input device
        static CAudioDeviceInfo getDefaultInputDevice();

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAudioDeviceInfo &compareValue) const;

        //! Device type as string
        static const QString &deviceTypeToString(DeviceType t);

    private:
        int m_type = static_cast<int>(Unknown); //!< Device type, @see CAudioDeviceInfo::DeviceType
        QString m_deviceName; //!< Device name
        CIdentifier m_identifier; //!< We use a DBus based system. Hence an audio device can reside on a different computers, this here is its name

        SWIFT_METACLASS(
            CAudioDeviceInfo,
            SWIFT_METAMEMBER(type),
            SWIFT_METAMEMBER(deviceName),
            SWIFT_METAMEMBER(identifier));
    };
} // namespace swift::misc::audio

Q_DECLARE_METATYPE(swift::misc::audio::CAudioDeviceInfo)
Q_DECLARE_METATYPE(swift::misc::audio::CAudioDeviceInfo::DeviceType)

#endif // SWIFT_MISC_AUDIO_AUDIODEVICE_H
