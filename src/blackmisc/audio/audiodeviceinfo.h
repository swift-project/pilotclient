/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_AUDIODEVICE_H
#define BLACKMISC_AUDIO_AUDIODEVICE_H

#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QAudioDeviceInfo>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Audio
    {
        /*!
         * Value object encapsulating information of a audio device.
         * If you want to safe this object, use the name instead of the index, since the index can change after
         * a restart.
         */
        class BLACKMISC_EXPORT CAudioDeviceInfo : public CValueObject<CAudioDeviceInfo>
        {
        public:
            //! Type
            enum DeviceType
            {
                InputDevice,
                OutputDevice,
                Unknown
            };

            //!
            //! Default constructor.
            CAudioDeviceInfo();

            //! Constructor.
            CAudioDeviceInfo(DeviceType type, const QString &name);

            //! Get the device name
            const QString &getName() const { return m_deviceName; }

            //! Host name
            const QString &getHostName() const { return m_hostName; }

            //! Type
            DeviceType getType() const { return m_type; }

            //! Input device
            bool isInputDevice()  const { return this->getType() == InputDevice; }

            //! Output device
            bool isOutputDevice() const { return this->getType() == OutputDevice; }

            //! Valid audio device object?
            bool isValid() const { return !m_deviceName.isEmpty(); }

            //! Is this a default device?
            bool isDefault() const { return m_deviceName == "default"; }

            //! Convert the Qt type
            static DeviceType fromQtMode(QAudio::Mode m);

            //! Default output device
            static CAudioDeviceInfo getDefaultOutputDevice()
            {
                return CAudioDeviceInfo(OutputDevice, "default");
            }

            //! Default input device
            static CAudioDeviceInfo getDefaultInputDevice()
            {
                return CAudioDeviceInfo(InputDevice, "default");
            }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            DeviceType m_type = Unknown; //!< Device type, @see CAudioDeviceInfo::DeviceType
            QString m_deviceName;        //!< Device name
            QString m_hostName;          //!< We use a DBus based system. Hence an audio device can reside on a differen computers, this here is its name

            BLACK_METACLASS(
                CAudioDeviceInfo,
                BLACK_METAMEMBER(type),
                BLACK_METAMEMBER(deviceName),
                BLACK_METAMEMBER(hostName)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfo)
Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDeviceInfo::DeviceType)

#endif // guard
