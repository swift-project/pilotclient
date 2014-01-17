/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "vaudiodevice.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QtNetwork/QHostInfo>
#include <tuple>

namespace BlackMisc
{
    namespace Voice
    {
        /*
         * Constructor
         */
        CAudioDevice::CAudioDevice() :
            m_type(Unknown), m_deviceIndex(invalidDevice()),
            m_deviceName(""), m_hostName(CAudioDevice::hostName())
        {
            // void
        }

        /*
         * Constructor
         */
        CAudioDevice::CAudioDevice(DeviceType type, const qint16 index, const QString &name) :
            m_type(type), m_deviceIndex(index),
            m_deviceName(name), m_hostName(CAudioDevice::hostName())
        {
            // void
        }

        /*
         * Host name
         */
        QString CAudioDevice::hostName()
        {
            QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
            return hostInfo.localHostName();
        }

        /*
         * Same device?
         */
        bool CAudioDevice::operator ==(const CAudioDevice &other) const
        {
            if (&other == this) return true;
            if (m_deviceIndex == other.m_deviceIndex && m_type == other.m_type) return true;

            // otherwise
            return false;
        }

        /*
         * Unequal?
         */
        bool CAudioDevice::operator !=(const CAudioDevice &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAudioDevice::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(static_cast<uint>(m_type));
            hashs << qHash(m_deviceIndex);
            hashs << qHash(m_deviceName);
            hashs << qHash(m_hostName);
            return BlackMisc::calculateHash(hashs, "CAudioDevice");
        }

        /*
         * As String
         */
        QString CAudioDevice::convertToQString(bool /* i18n */) const
        {
            if (this->m_hostName.isEmpty()) return m_deviceName;
            QString s(this->m_deviceName);
            s.append(" [");
            s.append(this->hostName());
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
         * Compare
         */
        int CAudioDevice::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAudioDevice &>(otherBase);

            const auto lhs = std::tie(this->m_type, this->m_deviceIndex, this->m_deviceName, this->m_hostName);
            const auto rhs = std::tie(other.m_type, other.m_deviceIndex, other.m_deviceName, other.m_hostName);

            if (lhs < rhs) { return -1; }
            if (lhs > rhs) { return 1; }
            return 0;
        }

        /*
         * Marshall to DBus
         */
        void CAudioDevice::marshallToDbus(QDBusArgument &argument) const
        {
            argument << static_cast<uint>(m_type);
            argument << m_deviceIndex;
            argument << m_deviceName;
            argument << m_hostName;
        }

        /*
         * Unmarshall from DBus
         */
        void CAudioDevice::unmarshallFromDbus(const QDBusArgument &argument)
        {
            uint t;
            argument >> t;
            this->m_type = static_cast<DeviceType>(t);
            argument >> m_deviceIndex;
            argument >> m_deviceName;
            argument >> m_hostName;
        }

        /*
         * Register
         */
        void CAudioDevice::registerMetadata()
        {
            qRegisterMetaType<CAudioDevice>();
            qDBusRegisterMetaType<CAudioDevice>();
        }

    } // Voice
} // BlackMisc
