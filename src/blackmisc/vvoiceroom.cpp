/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "vvoiceroom.h"
#include "blackmisc/blackmiscfreefunctions.h"

#include <QChar>
#include <QStringList>

namespace BlackMisc
{
    namespace Voice
    {
        CVoiceRoom::CVoiceRoom(const QString &serverSpec) : m_hostname(""), m_channel("")
        {
            if (serverSpec.contains("/"))
            {
                QStringList splittedSpec = serverSpec.split("/");
                m_hostname = splittedSpec.at(0);
                m_channel = splittedSpec.at(1);
            }
        }

        /*
         * Equal?
         */
        bool CVoiceRoom::operator ==(const CVoiceRoom &other) const
        {
            if (&other == this)
            {
                return true;
            }

            if (m_hostname == other.m_hostname && m_channel == other.m_channel)
            {
                return true;
            }

            // otherwise
            return false;
        }

        /*
         * Unequal?
         */
        bool CVoiceRoom::operator !=(const CVoiceRoom &other) const
        {
            return !((*this) == other);
        }

        uint CVoiceRoom::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(m_hostname);
            hashs << qHash(m_channel);
            return BlackMisc::calculateHash(hashs, "CVoiceRoom");
        }

        void CVoiceRoom::registerMetadata()
        {
            qRegisterMetaType<CVoiceRoom>();
            qDBusRegisterMetaType<CVoiceRoom>();
        }

        QString CVoiceRoom::convertToQString(bool /* i18n */ ) const
        {
            if (m_hostname.isEmpty() || m_channel.isEmpty()) return "Unknown";
            QString s = m_hostname;
            s.append("/").append(m_channel);
            return s;
        }

        /*
         * Marshall to DBus
         */
        void CVoiceRoom::marshallToDbus(QDBusArgument &argument) const
        {
            argument << m_hostname;
            argument << m_channel;
        }

        /*
         * Unmarshall from DBus
         */
        void CVoiceRoom::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> m_hostname;
            argument >> m_channel;
        }


    } // Voice
} // BlackMisc
