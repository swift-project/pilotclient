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
        CVoiceRoom::CVoiceRoom(const QString &serverUrl, bool connected) :
            m_hostname(""), m_channel(""), m_connected(connected), m_audioPlaying(false)
        {
            if (serverUrl.contains("/"))
            {
                QString url = serverUrl.trimmed().toLower();
                url.replace(CVoiceRoom::protocolComplete(), "");
                url.replace(CVoiceRoom::protocol(), "");
                QStringList splittedSpec = serverUrl.split("/");
                m_hostname = splittedSpec.at(0);
                m_channel = splittedSpec.at(1);
            }
        }

        /*
         * Equal?
         */
        bool CVoiceRoom::operator ==(const CVoiceRoom &other) const
        {
            if (&other == this) return true;
            return (m_hostname == other.m_hostname &&
                    m_channel == other.m_channel &&
                    m_connected == other.m_connected &&
                    m_audioPlaying == other.m_audioPlaying);
        }

        /*
         * Unequal?
         */
        bool CVoiceRoom::operator !=(const CVoiceRoom &other) const
        {
            return !((*this) == other);
        }

        /*
         * Value hash
         */
        uint CVoiceRoom::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(m_hostname);
            hashs << qHash(m_channel);
            hashs << qHash(m_connected);
            hashs << qHash(m_audioPlaying);
            return BlackMisc::calculateHash(hashs, "CVoiceRoom");
        }

        /*
         * Metadata
         */
        void CVoiceRoom::registerMetadata()
        {
            qRegisterMetaType<CVoiceRoom>();
            qDBusRegisterMetaType<CVoiceRoom>();
        }

        /*
         * To string
         */
        QString CVoiceRoom::convertToQString(bool /* i18n */) const
        {
            if (!this->isValid()) return "Invalid";
            QString s = this->getVoiceRoomUrl(false);
            s.append(this ->isConnected() ? " connected" : " unconnected");
            if (this->m_audioPlaying) s.append(" playing");
            return s;
        }

        /*
         * Marshall to DBus
         */
        void CVoiceRoom::marshallToDbus(QDBusArgument &argument) const
        {
            argument << m_hostname;
            argument << m_channel;
            argument << m_connected;
            argument << m_audioPlaying;
        }

        /*
         * Unmarshall from DBus
         */
        void CVoiceRoom::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> m_hostname;
            argument >> m_channel;
            argument >> m_connected;
            argument >> m_audioPlaying;
        }

        /*
         * Server URL
         */
        QString CVoiceRoom::getVoiceRoomUrl(bool noProtocol) const
        {
            if (!this->isValid()) return "";
            QString url(noProtocol ? "" : CVoiceRoom::protocolComplete());
            url.append(this->m_hostname);
            url.append("/");
            url.append(this->m_channel);
            return url;
        }

        /*
         * ATIS voice channel
         */
        bool CVoiceRoom::isAtis() const
        {
            return (this->m_channel.contains("ATIS", Qt::CaseInsensitive));
        }
    } // Voice
} // BlackMisc
