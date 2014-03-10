/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AUDIODEVICE_H
#define BLACKMISC_AUDIODEVICE_H

/*!
    \file
*/

#include "blackmiscfreefunctions.h"
#include "valueobject.h"
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
        class CAudioDevice : public BlackMisc::CValueObject
        {
        public:

            //! \brief Type
            enum DeviceType
            {
                InputDevice,
                OutputDevice,
                Unknown
            };

            /*!
             * Default constructor.
             * If m_deviceIndex is -1, default should be used. However on Windows this doesnt work. Needs
             * to be checked in Vatlib.
             */
            CAudioDevice();

            //! \brief Constructor.
            CAudioDevice(DeviceType type, const qint16 index, const QString &getName);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Get the device index
            qint16 getIndex() const { return m_deviceIndex; }

            //! Get the device name
            const QString &getName() const { return m_deviceName; }

            //! \brief Type
            DeviceType getType() const { return m_type; }

            //! \brief Valid audio device object?
            bool isValid() const { return m_deviceIndex >= -1 && !m_deviceName.isEmpty(); }

            //! \brief Equal operator ==
            bool operator ==(const CAudioDevice &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAudioDevice &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \brief Register metadata
            static void registerMetadata();

            //! \brief Device index for default device
            static qint16 defaultDeviceIndex() {return -1;}

            //! \brief Invalid device index
            static qint16 invalidDeviceIndex() {return -2;}

            //! \brief default output device
            static CAudioDevice getDefaultOutputDevice()
            {
                return CAudioDevice(OutputDevice, defaultDeviceIndex(), "default");
            }

            //! \brief default input device
            static CAudioDevice getDefaultInputDevice()
            {
                return CAudioDevice(InputDevice, defaultDeviceIndex(), "default");
            }

        protected:

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAudioDevice)
            //! \brief Device type, @see CAudioDevice::DeviceType
            DeviceType m_type;
            /*!
             * deviceIndex is the number is the reference for the VVL. The device is selected by this index.
             * The managing class needs to take care, that indexes are valid.
             */
            qint16 m_deviceIndex;
            //! \brief Device name
            QString m_deviceName;
            //! \brief We use a DBus based system. Hence an audio device can reside on a differen computers, this here is its name
            QString m_hostName;

        private:
            /*!
             * \brief Own host name
             * \return
             */
            static QString hostName();
        };

    } // Voice
} // BlackMisc

BLACK_DBUS_ENUM_MARSHALLING(BlackMisc::Audio::CAudioDevice::DeviceType)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Audio::CAudioDevice, (o.m_type, o.m_deviceIndex, o.m_deviceName, o.m_hostName))
Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDevice)

#endif // guard
