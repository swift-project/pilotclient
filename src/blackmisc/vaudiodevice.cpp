/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "vaudiodevice.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Voice
    {
        bool CAudioDevice::operator ==(const CAudioDevice &other) const
        {
            if (&other == this)
            {
                return true;
            }

            if (m_deviceIndex == other.m_deviceIndex)
            {
                return true;
            }

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

        uint CAudioDevice::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(m_deviceIndex);
            hashs << qHash(m_deviceName);
            return BlackMisc::calculateHash(hashs, "CAudioDevice");
        }

        void CAudioDevice::registerMetadata()
        {
            qRegisterMetaType<CAudioDevice>();
            qDBusRegisterMetaType<CAudioDevice>();
        }

        QString CAudioDevice::convertToQString(bool i18n) const
        {
            return m_deviceName;
        }

        /*
         * Marshall to DBus
         */
        void CAudioDevice::marshallToDbus(QDBusArgument &argument) const
        {
            argument << m_deviceIndex;
            argument << m_deviceName;
        }

        /*
         * Unmarshall from DBus
         */
        void CAudioDevice::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> m_deviceIndex;
            argument >> m_deviceName;
        }

        void CInputAudioDevice::registerMetadata()
        {
            qRegisterMetaType<CInputAudioDevice>();
            qDBusRegisterMetaType<CInputAudioDevice>();
        }

        void COutputAudioDevice::registerMetadata()
        {
            qRegisterMetaType<COutputAudioDevice>();
            qDBusRegisterMetaType<COutputAudioDevice>();
        }

    } // Voice
} // BlackMisc
