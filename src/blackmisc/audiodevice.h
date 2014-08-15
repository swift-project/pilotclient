/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIODEVICE_H
#define BLACKMISC_AUDIODEVICE_H

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

            //! Type
            enum DeviceType : uint
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

            //! Constructor.
            CAudioDevice(DeviceType type, const qint16 index, const QString &getName);

            //! Get the device index
            qint16 getIndex() const { return m_deviceIndex; }

            //! Get the device name
            const QString &getName() const { return m_deviceName; }

            //! Host name
            const QString &getHostName() const { return m_hostName; }

            //! Type
            DeviceType getType() const { return m_type; }

            //! Valid audio device object?
            bool isValid() const { return m_deviceIndex >= -1 && !m_deviceName.isEmpty(); }

            //! Equal operator ==
            bool operator ==(const CAudioDevice &other) const;

            //! Unequal operator !=
            bool operator !=(const CAudioDevice &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

            //! Device index for default device
            static qint16 defaultDeviceIndex() {return -1;}

            //! Invalid device index
            static qint16 invalidDeviceIndex() {return -2;}

            //! Default output device
            static CAudioDevice getDefaultOutputDevice()
            {
                return CAudioDevice(OutputDevice, defaultDeviceIndex(), "default");
            }

            //! Default input device
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
            //! Device type, @see CAudioDevice::DeviceType
            DeviceType m_type;
            /*!
             * deviceIndex is the number is the reference for the VVL. The device is selected by this index.
             * The managing class needs to take care, that indexes are valid.
             */
            qint16 m_deviceIndex;
            //! Device name
            QString m_deviceName;
            //! We use a DBus based system. Hence an audio device can reside on a differen computers, this here is its name
            QString m_hostName;
        };

    } // Audio
} // BlackMisc

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Audio::CAudioDevice, (o.m_type, o.m_deviceIndex, o.m_deviceName, o.m_hostName))
Q_DECLARE_METATYPE(BlackMisc::Audio::CAudioDevice)

#endif // guard
