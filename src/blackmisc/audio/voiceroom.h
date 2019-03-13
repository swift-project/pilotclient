/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_VOICEROOM_H
#define BLACKMISC_AUDIO_VOICEROOM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Audio
    {
        //! Value object encapsulating information of a voice room
        class BLACKMISC_EXPORT CVoiceRoom : public CValueObject<CVoiceRoom>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexHostname = CPropertyIndex::GlobalIndexCVoiceRoom,
                IndexChannel,
                IndexUrl,
                IndexConnected,
                IndexAudioPlaying
            };

            //! Default constructor.
            CVoiceRoom() : m_connected(false), m_audioPlaying(false) {}

            //! Constructor.
            CVoiceRoom(const QString &hostname, const QString &channel) :
                m_hostname(hostname), m_channel(channel), m_connected(false), m_audioPlaying(false) {}

            //! Constructor.
            CVoiceRoom(const QString &voiceRoomUrl, bool connected = false);

            //! Get the host name
            const QString &getHostname() const { return m_hostname; }

            //! Get the voice room channel
            const QString &getChannel() const { return m_channel; }

            //! Set the host name
            void setHostName(const QString &hostName) { m_hostname = hostName; }

            //! Set the voice channel
            void setChannel(const QString &channel) { m_channel = channel; }

            //! Server URL
            //! \param noProtocol either with (pseudo) protocol prefix or without
            QString getVoiceRoomUrl(bool noProtocol = true) const;

            //! Set voice room URL
            void setVoiceRoomUrl(const QString &serverUrl);

            //! Valid voice room object?
            bool isValid() const { return !m_hostname.isEmpty() && !m_channel.isEmpty(); }

            //! Is connected?
            bool isConnected() const { return this->isValid() && m_connected; }

            //! Set connection status
            void setConnected(bool isConnected) { m_connected = isConnected; }

            //! Is audio playing in this room?
            bool isAudioPlaying() const { return m_audioPlaying; }

            //! Set audio playing
            void setAudioPlaying(bool playing)  { m_audioPlaying = playing; }

            //! Is ATIS voice channel
            bool isAtis() const;

            //! Can talk to channel
            bool canTalkTo() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Protocol prefix "vvl"
            static const QString &protocol() { static QString p("vvl"); return p; }

            //! Protocol
            //! \return with protocol prefix or without
            static const QString &protocolComplete() { static QString p("vvl://"); return p; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            QString m_hostname;
            QString m_channel;
            bool m_connected;
            bool m_audioPlaying;

            BLACK_METACLASS(
                CVoiceRoom,
                BLACK_METAMEMBER(hostname),
                BLACK_METAMEMBER(channel),
                BLACK_METAMEMBER(connected),
                BLACK_METAMEMBER(audioPlaying)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Audio::CVoiceRoom)

#endif // guard
