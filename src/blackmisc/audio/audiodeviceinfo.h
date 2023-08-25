// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AUDIO_AUDIODEVICE_H
#define BLACKMISC_AUDIO_AUDIODEVICE_H

#include "blackmisc/identifier.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QAudioDeviceInfo>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Audio, CAudioDeviceInfo)

namespace BlackMisc::Audio
{
    /*!
     * Value object encapsulating information of a audio device.
     * If you want to safe this object, use the name instead of the index, since the index can change after
     * a restart.
     */
    class BLACKMISC_EXPORT CAudioDeviceInfo : public CValueObject<CAudioDeviceInfo>
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

        //! Convert the Qt type
        static DeviceType fromQtMode(QAudio::Mode m);

        //! Default output device
        static CAudioDeviceInfo getDefaultOutputDevice();

        //! Default input device
        static CAudioDeviceInfo getDefaultInputDevice();

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAudioDeviceInfo &compareValue) const;

        //! Device type as string
        static const QString &deviceTypeToString(DeviceType t);

    private:
        int m_type = static_cast<int>(Unknown); //!< Device type, @see CAudioDeviceInfo::DeviceType
        QString m_deviceName; //!< Device name
        CIdentifier m_identifier; //!< We use a DBus based system. Hence an audio device can reside on a different computers, this here is its name

        BLACK_METACLASS(
            CAudioDeviceInfo,
            BLACK_METAMEMBER(type),
            BLACK_METAMEMBER(deviceName),
            BLACK_METAMEMBER(identifier)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfo)
Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfo::DeviceType)

#endif // guard
