/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "audiodevice.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    namespace Audio
    {
        /*
         * Constructor
         */
        CAudioDevice::CAudioDevice() :
            m_type(Unknown), m_deviceIndex(invalidDeviceIndex()),
            m_deviceName(""), m_hostName(BlackMisc::localHostName())
        {
            // void
        }

        /*
         * Constructor
         */
        CAudioDevice::CAudioDevice(DeviceType type, const qint16 index, const QString &name) :
            m_type(type), m_deviceIndex(index),
            m_deviceName(name), m_hostName(BlackMisc::localHostName())
        {
            // void
        }

        /*
         * Compare
         */
        int CAudioDevice::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAudioDevice &>(otherBase);
            return compare(TupleConverter<CAudioDevice>::toTuple(*this), TupleConverter<CAudioDevice>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAudioDevice::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAudioDevice>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAudioDevice::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAudioDevice>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CAudioDevice::getValueHash() const
        {
            return qHash(TupleConverter<CAudioDevice>::toTuple(*this));
        }

        /*
         * Equal?
         */
        bool CAudioDevice::operator ==(const CAudioDevice &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAudioDevice>::toTuple(*this) == TupleConverter<CAudioDevice>::toTuple(other);
        }


        /*
         * Unequal?
         */
        bool CAudioDevice::operator !=(const CAudioDevice &other) const
        {
            return !((*this) == other);
        }

        /*
         * As String
         */
        QString CAudioDevice::convertToQString(bool /* i18n */) const
        {
            if (this->m_hostName.isEmpty()) return m_deviceName;
            QString s(this->m_deviceName);
            s.append(" [");
            s.append(this->getHostName());
            s.append("]");
            return s;
        }

        /*
         * metaTypeId
         */
        int CAudioDevice::getMetaTypeId() const
        {
            return qMetaTypeId<CAudioDevice>();
        }

        /*
         * is a
         */
        bool CAudioDevice::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAudioDevice>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register
         */
        void CAudioDevice::registerMetadata()
        {
            qRegisterMetaType<CAudioDevice>();
            qDBusRegisterMetaType<CAudioDevice>();
        }

        /*
         * Members
         */
        const QStringList &CAudioDevice::jsonMembers()
        {
            return TupleConverter<CAudioDevice>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CAudioDevice::toJson() const
        {
            return BlackMisc::serializeJson(CAudioDevice::jsonMembers(), TupleConverter<CAudioDevice>::toTuple(*this));
        }

        /*
         * From Json
         */
        void CAudioDevice::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAudioDevice::jsonMembers(), TupleConverter<CAudioDevice>::toTuple(*this));
        }

    } // Voice
} // BlackMisc
