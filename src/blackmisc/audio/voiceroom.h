/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QString>

#ifndef BLACKMISC_AUDIO_VOICEROOM_H
#define BLACKMISC_AUDIO_VOICEROOM_H

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
                IndexHostname = BlackMisc::CPropertyIndex::GlobalIndexCVoiceRoom,
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

            /*!
             * Server URL
             * \param noProtocol either with (pseudo) protocol prefix or without
             * \return
             */
            QString getVoiceRoomUrl(bool noProtocol = true) const;

            //! Set voice room URL
            void setVoiceRoomUrl(const QString &serverUrl);

            //! Valid voice room object?
            bool isValid() const { return !this->m_hostname.isEmpty() &&  !this->m_channel.isEmpty(); }

            //! Is connected?
            bool isConnected() const { return this->isValid() && this->m_connected; }

            //! Set connection status
            void setConnected(bool isConnected) { this->m_connected = isConnected; }

            //! Is audio playing in this room?
            bool isAudioPlaying() const { return this->m_audioPlaying; }

            //! Set audio playing
            void setAudioPlaying(bool playing)  { this->m_audioPlaying = playing; }

            //! Is ATIS voice channel
            bool isAtis() const;

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Protocol prefix "vvl"
            static const QString &protocol() { static QString p("vvl"); return p; }

            /*!
             * Protocol
             * \return with protocol prefix or without
             */
            static const QString &protocolComplete() { static QString p("vvl://"); return p; }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CVoiceRoom)
            QString m_hostname;
            QString m_channel;
            bool m_connected;
            bool m_audioPlaying;
        };
    } // Voice
} // BlackMisc

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Audio::CVoiceRoom, (o.m_hostname, o.m_channel, o.m_connected, o.m_audioPlaying))
Q_DECLARE_METATYPE(BlackMisc::Audio::CVoiceRoom)

#endif // guard
